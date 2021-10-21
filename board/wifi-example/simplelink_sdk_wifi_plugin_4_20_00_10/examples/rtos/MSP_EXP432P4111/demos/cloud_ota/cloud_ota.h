/*
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//*****************************************************************************
// Includes
//*****************************************************************************
// Standard includes
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//	App includes
#include "uart_term.h"

//	TI-Driver includes
#include <ti/drivers/GPIO.h>
#include <ti/drivers/net/wifi/simplelink.h>

#ifndef NORTOS_SUPPORT
//	POSIX includes
#include "pthread.h"
#endif

//*****************************************************************************
// Defines
//*****************************************************************************
#define APPLICATION_NAME        "OTA APP EX0"
#define APPLICATION_VERSION     "0.10.00.03"
#define SPAWN_TASK_PRIORITY     (9)
#define TASK_STACK_SIZE         (3 * 1024)

#define OTA_DBG_PRINT UART_PRINT

/* Continue is case of NO_UPDATE, OLDER_VERSION, Initialize error without
 * pending commit(< 20% ping)
 */
//#define OTA_LOOP_TESTING
/* Don't wait for external trigger to start the OTA, run OTA after
 * 5 ping sequences
 */
//#define DISABLE_OTA_SWITCH_TRIGGER
#define ASYNC_EVT_TIMEOUT                   (5000)      /* In msecs */
#define LED_TOGGLE_TIMEOUT                  (500)       /* In msecs */
#define PASS_PERCENTAGE                     (10)
#define SL_STOP_TIMEOUT                     (200)
#define OSI_STACK_SIZE                      (4048)
#define PROVISIONING_APP_VERSION            "1.0.16"
#define PROVISIONING_INACTIVITY_TIMEOUT     600     /* Provisioning inactivity
                                                       timeout in seconds */
#define RECONNECTION_ESTABLISHED_TIMEOUT    15000   /* 15 seconds */

//*****************************************************************************
// Typedefs
//*****************************************************************************
/*!
 *  \brief  Application's states
 */
typedef enum
{
    APP_STATE_STARTING, APP_STATE_WAIT_FOR_CONNECTION, APP_STATE_WAIT_FOR_IP,

    APP_STATE_PROVISIONING_IN_PROGRESS, APP_STATE_PROVISIONING_WAIT_COMPLETE,

    APP_STATE_PINGING_GW,

    APP_STATE_OTA_RUN,

    APP_STATE_ERROR, APP_STATE_MAX
} e_AppState;

/*!
 *  \brief  Application's events
 */
typedef enum
{
    APP_EVENT_NULL,
    APP_EVENT_STARTED,
    APP_EVENT_CONNECTED,
    APP_EVENT_IP_ACQUIRED,
    APP_EVENT_DISCONNECT, /* used also for IP lost */
    APP_EVENT_PROVISIONING_STARTED,
    APP_EVENT_PROVISIONING_SUCCESS,
    APP_EVENT_PROVISIONING_STOPPED,
    APP_EVENT_PING_COMPLETE,
    APP_EVENT_OTA_START,
    APP_EVENT_CONTINUE,
    APP_EVENT_OTA_CHECK_DONE,
    APP_EVENT_OTA_DOWNLOAD_DONE,
    APP_EVENT_OTA_ERROR,
    APP_EVENT_TIMEOUT,
    APP_EVENT_ERROR,
    APP_EVENT_RESTART,
    APP_EVENT_MAX
} e_AppEvent;

/*
 *  \brief  Application state's context
 */
typedef struct
{
    e_AppState currentState; /* Current state of the application */
    volatile uint32_t pendingEvents; /* Events pending to be processed */
    uint8_t role; /* SimpleLink's role - STATION/AP/P2P */
    uint32_t asyncEvtTimeout; /* Timeout value*/
    PlatformTimeout_t PlatformTimeout_Led;
} s_AppContext;

/*!
 *  \brief  Application data
 */
typedef struct
{
    SlNetAppPingReport_t pingReport; /* Variable to store the ping report */
    uint32_t gatewayIP; /* Variable to store the gateway IP
                         * address
                         */
} s_AppData;

/*!
 *  \brief  Function pointer to the event handler
 */
typedef int32_t (*fptr_EventHandler)();

/*!
 *  \brief  Entry in the lookup table
 */
typedef struct
{
    fptr_EventHandler p_evtHndl; /* Pointer to the event handler */
    e_AppState nextState; /* Next state of the application */
} s_TblEntry;

extern void * mainThread(void *pvParameters);
extern int32_t OtaInit();
extern int32_t OtaCheckAndDoCommit();
extern int32_t HandlePingComplete();
extern int32_t ProcessRestartMcu();
extern int32_t OtaImageTestingAndReset();
extern int32_t OtaRunStep();
