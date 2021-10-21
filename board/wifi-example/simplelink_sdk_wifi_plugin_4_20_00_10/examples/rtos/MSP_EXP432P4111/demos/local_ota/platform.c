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
//
//! \addtogroup local_ota
//! @{
//
//*****************************************************************************
/* TI-DRIVERS Header files */
#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti_drivers_config.h>
#include <ota_extended.h>

/* POSIX Header files */
#include <pthread.h>
#include <time.h>

/* Example/Board Header files */
#include "platform.h"

extern void * InitTerm();

#if defined(__TI_COMPILER_VERSION__)
#pragma RETAIN(appTransferKey)
#pragma LOCATION(appTransferKey, SHARED_MEMORY_LOCATION)
volatile uint32_t appTransferKey;
#elif defined(__GNUC__)
volatile uint32_t __attribute__((section (".shared_memory"))) appTransferKey;
#elif defined(__IAR_SYSTEMS_ICC__)
__no_init volatile uint32_t appTransferKey @ SHARED_MEMORY_LOCATION;
#endif

//****************************************************************************
//                            MAIN FUNCTION
//****************************************************************************
void Platform_TimerInit(void (*timerIntHandler)(sigval val),
                        timer_t *timerId)
{
    sigevent sev;

    /* Create Timer */
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_notify_function = timerIntHandler;
    timer_create(CLOCK_MONOTONIC, &sev, timerId);
}

void Platform_TimerStart(uint32_t asyncEvtTimeoutMsec,
                         timer_t timerId,
                         uint8_t periodic)
{
    struct itimerspec value;

    /* set the timeout */
    value.it_value.tv_sec = (asyncEvtTimeoutMsec / 1000);
    value.it_value.tv_nsec = (asyncEvtTimeoutMsec % 1000) * 1000000;

    value.it_value.tv_sec += (value.it_value.tv_nsec / 1000000000);
    value.it_value.tv_nsec = value.it_value.tv_nsec % 1000000000;

    if(periodic)
    {
        /* set as periodic timer */
        value.it_interval.tv_sec = value.it_value.tv_sec;
        value.it_interval.tv_nsec = value.it_value.tv_nsec;
    }
    else
    {
        /* set as one shot timer */
        value.it_interval.tv_sec = 0;
        value.it_interval.tv_nsec = 0;
    }

    /* kick the timer */
    timer_settime(timerId, 0, &value, NULL);
}

void Platform_TimerStop(timer_t timerId)
{
    struct itimerspec value;

    /* stop timer */
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_nsec = 0;
    value.it_value.tv_sec = 0;
    value.it_value.tv_nsec = 0;
    timer_settime(timerId, 0, &value, NULL);
}

void Platform_TimerInterruptClear(void)
{
}

void Platform_processOTAImage(void)
{
    /* Pass the control to the boot code */
    appTransferKey = APPLICATION_PERFORM_BSL;
}
