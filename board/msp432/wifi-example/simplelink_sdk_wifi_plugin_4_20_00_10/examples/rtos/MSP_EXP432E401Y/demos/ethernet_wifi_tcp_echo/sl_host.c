/*
 * Copyright (c) 2017, Texas Instruments Incorporated
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

/*
 *  ======== sl_host.c ========
 */

/* POSIX Header files */
#include <pthread.h>
#include <unistd.h>
#include <mqueue.h>

/* TI-Driver includes */
#include <ti/display/Display.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>

/* Simplelink includes */
#include <ti/drivers/net/wifi/simplelink.h>

/* Sockets */
#include <ti/drivers/net/wifi/sl_socket.h>

/* Wifi plugin */
#include <ti/drivers/net/wifi/slnetifwifi.h>

/* Common interface includes */
#include "network_if.h"

/* Application includes */
#include "ti_drivers_config.h"
#include "ethernet_wifi_tcp_echo.h"

extern Display_Handle display;
extern void * WiFiTCPHandler(void *args);

/* Expiration value for the timer that is being used to toggle the Led.
 */
#define TIMER_EXPIRATION_VALUE   100 * 1000000

#define SPAWN_TASK_PRIORITY      9
#define THREADSTACKSIZE          4096
#define TASKSTACKSIZE            2048

timer_t g_timer;
unsigned short g_usTimerInts;

/* AP Security Parameters
 */
SlWlanSecParams_t SecurityParams = { 0 };

//*****************************************************************************
//
//! Periodic Timer Interrupt Handler
//!
//! \param None
//!
//! \return None
//
//*****************************************************************************
void TimerPeriodicIntHandler(sigval val)
{
    /* Increment our interrupt counter.
     */
    g_usTimerInts++;

    if(!(g_usTimerInts & 0x1))
    {
        /* Turn Led Off
         */
        GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
    }
    else
    {
        /* Turn Led On
         */
        GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
    }
}

//*****************************************************************************
//
//! Function to configure and start timer to blink the LED while device is
//! trying to connect to an AP
//!
//! \param none
//!
//! return none
//
//*****************************************************************************
void LedTimerConfigNStart()
{
    struct itimerspec value;
    sigevent sev;

    /* Create Timer
     */
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_notify_function = &TimerPeriodicIntHandler;
    timer_create(2, &sev, &g_timer);

    /* start timer
     */
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_nsec = TIMER_EXPIRATION_VALUE;
    value.it_value.tv_sec = 0;
    value.it_value.tv_nsec = TIMER_EXPIRATION_VALUE;

    timer_settime(g_timer, 0, &value, NULL);
}

//*****************************************************************************
//
//! Disable the LED blinking Timer as Device is connected to AP
//!
//! \param none
//!
//! return none
//
//*****************************************************************************
void LedTimerDeinitStop()
{
    /* Disable the LED blinking Timer as Device is connected to AP.
     */
    timer_delete(g_timer);
}

//*****************************************************************************
//
//! This function connect the TCP echo device to an AP with the SSID which was
//! configured in SSID_NAME definition which can be found in Network_if.h file,
//! if the device can't connect to to this AP a request from the user for other
//! SSID will appear.
//!
//! \param  none
//!
//! \return None
//!
//*****************************************************************************
int32_t Echo_IF_Connect()
{
    int32_t lRetVal;
    char SSID_Remote_Name[32];
    int8_t Str_Length;

    memset(SSID_Remote_Name, '\0', sizeof(SSID_Remote_Name));
    Str_Length = strlen(SSID_NAME);

    if(Str_Length)
    {
        /* Copy the Default SSID to the local variable
         */
        strncpy(SSID_Remote_Name, SSID_NAME, Str_Length);
    }

    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
    GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_OFF);
    GPIO_write(Board_GPIO_LED2, Board_GPIO_LED_OFF);

    /* Reset The state of the machine
     */
    Network_IF_ResetMCUStateMachine();

    /* Start the driver
     */
    lRetVal = Network_IF_InitDriver(ROLE_STA);
    if(lRetVal < 0)
    {
        Display_printf(display, 0, 0, "Failed to start SimpleLink Device\n\r",
                       lRetVal);
        return(-1);
    }

    /* switch on Green LED to indicate Simplelink is properly up.
     */
    GPIO_write(Board_GPIO_LED2, Board_GPIO_LED_ON);

    /* Start Timer to blink Red LED till AP connection
     */
    LedTimerConfigNStart();

    /* Initialize AP security params
     */
    SecurityParams.Key = (signed char *) SECURITY_KEY;
    SecurityParams.KeyLen = strlen(SECURITY_KEY);
    SecurityParams.Type = SECURITY_TYPE;

    /* Connect to the Access Point
     */
    lRetVal = Network_IF_ConnectAP(SSID_Remote_Name, SecurityParams);
    if(lRetVal < 0)
    {
        Display_printf(display, 0, 0, "Connection to an AP failed\n\r");
        return(-1);
    }

    /* Disable the LED blinking Timer as Device is connected to AP.
     */
    LedTimerDeinitStop();

    /* Switch ON RED LED to indicate that Device acquired an IP.
     */
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);

    sleep(1);

    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
    GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_OFF);
    GPIO_write(Board_GPIO_LED2, Board_GPIO_LED_OFF);

    return(0);
}

/*
 *  ======== skHostStackThread ========
 *  SimpleLink Host Driver main thread function
 */
static void * slHostStackThread(void *args)
{
    pthread_t spawn_thread = (pthread_t) NULL;
    pthread_attr_t pAttrs_spawn;
    struct sched_param priParam;

    pthread_t handler_thread = (pthread_t) NULL;
    pthread_attr_t pAttrs_handler;
    struct sched_param handlerParam;

    int32_t retc = 0;
    int32_t status = 0;

    /* Create the sl_Task  */
    pthread_attr_init(&pAttrs_spawn);
    priParam.sched_priority = SPAWN_TASK_PRIORITY;
    retc = pthread_attr_setschedparam(&pAttrs_spawn, &priParam);
    retc |= pthread_attr_setstacksize(&pAttrs_spawn, TASKSTACKSIZE);
    retc |= pthread_attr_setdetachstate(&pAttrs_spawn, PTHREAD_CREATE_DETACHED);
    retc = pthread_create(&spawn_thread, &pAttrs_spawn, sl_Task, NULL);

    if(retc != 0)
    {
        Display_printf(display, 0, 0,
                       "slHostStackThread: failed to create sl_Task (%d)\n");
        while(1)
        {
            ;
        }
    }

    retc = sl_Start(0, 0, 0);
    if(retc < 0)
    {
        /* Handle Error */
        Display_printf(display, 0, 0,
                       "slHostStackThread: sl_Start failed (%d)\n");
        while(1)
        {
            ;
        }
    }

    retc = sl_Stop(SL_STOP_TIMEOUT);
    if(retc < 0)
    {
        /* Handle Error */
        Display_printf(display, 0, 0,
                       "slHostStackThread: sl_Stop failed (%d)\n");
        while(1)
        {
            ;
        }
    }

    if(retc < 0)
    {
        /* Handle Error */
        Display_printf(
            display, 0, 0,
            "slHostStackThread: Unable to retrieve device information failed (%d)\n");
        while(1)
        {
            ;
        }
    }

    /* Connect to AP */
    Echo_IF_Connect();
    Display_printf(display, 0, 0, "WiFi Interface connected and started\n");

    status = SlNetSock_init(0);
    if(status != 0)
    {
        Display_printf(display, 0, 0, "SlNetSock_init fail (%d)\n",
                       status);
    }

    status = SlNetIf_init(0);
    if(status != 0)
    {
        Display_printf(display, 0, 0, "SlNetIf_init fail (%d)\n",
                       status);
    }
    status = SlNetIf_add(SLNETIF_ID_1, "wifi0",
                         (const SlNetIf_Config_t *)&SlNetIfConfigWifi, 3);
    if(status != 0)
    {
        Display_printf(display, 0, 0, "SlNetIf_add fail (%d)\n",
                       status);
    }

    /* Create the TCP handler thread  */
    pthread_attr_init(&pAttrs_handler);
    handlerParam.sched_priority = 1;
    retc = pthread_attr_setschedparam(&pAttrs_handler, &handlerParam);
    retc |= pthread_attr_setstacksize(&pAttrs_handler, HANDLERSTACKSIZE);
    retc |= pthread_attr_setdetachstate(&pAttrs_handler,
                                        PTHREAD_CREATE_DETACHED);
    retc = pthread_create(&handler_thread, &pAttrs_handler, WiFiTCPHandler,
                          NULL);

    if(retc != 0)
    {
        Display_printf(display, 0, 0,
                       "slHostStackThread: failed to create sl_Task (%d)\n");
        while(1)
        {
            ;
        }
    }

    return (NULL);
}

/*
 * ======== ti_simplelink_host_config_Global_startupFxn ========
 * Called to start up the SimpleLink HOST. In BIOS, this can be called as a
 * BIOS startup
 * function, or from main(). In FreeRTOS, this should be called from main().
 */
void ti_simplelink_host_config_Global_startupFxn()
{
    pthread_t thread;
    pthread_attr_t pAttrs;
    struct sched_param priParam;
    int retc;
    int detachState;

    /* Set priority and stack size attributes */
    pthread_attr_init(&pAttrs);
    priParam.sched_priority = 1;

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&pAttrs, detachState);
    if(retc != 0)
    {
        /* pthread_attr_setdetachstate() failed */
        while(1)
        {
            ;
        }
    }

    pthread_attr_setschedparam(&pAttrs, &priParam);

    retc |= pthread_attr_setstacksize(&pAttrs, (int)2048);
    if(retc != 0)
    {
        /* pthread_attr_setstacksize() failed */
        while(1)
        {
            ;
        }
    }

    retc = pthread_create(&thread, &pAttrs, &slHostStackThread, NULL);
    if(retc != 0)
    {
        /* pthread_create() failed */
        while(1)
        {
            ;
        }
    }

    pthread_attr_destroy(&pAttrs);
}
