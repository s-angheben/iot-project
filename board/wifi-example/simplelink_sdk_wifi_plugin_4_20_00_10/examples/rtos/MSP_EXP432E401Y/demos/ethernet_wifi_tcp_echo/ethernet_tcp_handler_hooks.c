/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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

#include <ti/ndk/inc/netmain.h>
#include <ti/ndk/slnetif/slnetifndk.h>
#include <ti/net/slnetif.h>

#include <ti/display/Display.h>

#include "ethernet_wifi_tcp_echo.h"

/* POSIX Header files */
#include <pthread.h>

extern Display_Handle display;

/* Prototypes */
extern void * EthernetTCPHandler(void *args);

/*
 *  ======== netIPAddrHook ========
 *  User defined network IP address hook. Starts the EthernetTCPHandler thread.
 *
 */
void netIPAddrHook(uint32_t IPAddr,
                   unsigned int IfIdx,
                   unsigned int fAdd)
{
    static bool createTask = true;
    int32_t status = 0;
    int32_t retc = 0;

    pthread_t handler_thread = (pthread_t) NULL;
    pthread_attr_t pAttrs_handler;
    struct sched_param handlerParam;

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
    status = SlNetIf_add(SLNETIF_ID_2, "eth0",
                         (const SlNetIf_Config_t *)&SlNetIfConfigNDK, 4);
    if(status != 0)
    {
        Display_printf(display, 0, 0, "SlNetIf_add fail (%d)\n",
                       status);
    }

    if(fAdd && createTask)
    {
        /*
         *  Create the Task that farms out incoming TCP connections.
         *  arg0 will be the port that this task listens to.
         */
        pthread_attr_init(&pAttrs_handler);
        handlerParam.sched_priority = 1;
        retc = pthread_attr_setschedparam(&pAttrs_handler, &handlerParam);
        retc |= pthread_attr_setstacksize(&pAttrs_handler, HANDLERSTACKSIZE);
        retc |= pthread_attr_setdetachstate(&pAttrs_handler,
                                            PTHREAD_CREATE_DETACHED);
        retc |=
            pthread_create(&handler_thread, &pAttrs_handler,
                           EthernetTCPHandler,
                           NULL);
        if(retc != 0)
        {
            /* netOpenHook: Failed to create tcpHandler thread */
            while(1)
            {
                ;
            }
        }

        createTask = false;
    }
}

/*
 *  ======== netOpenHook ========
 *  NDK network open hook
 */
void netOpenHook()
{
}
