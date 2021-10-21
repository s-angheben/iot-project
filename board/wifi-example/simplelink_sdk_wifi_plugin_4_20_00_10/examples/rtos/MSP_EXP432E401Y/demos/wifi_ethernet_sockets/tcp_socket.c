/*
 * Copyright (c) 2014-2017, Texas Instruments Incorporated
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

#include <string.h>
#include <stdint.h>

/* POSIX Header files */
#include <pthread.h>
#include <unistd.h>

/* SlNetSock support */
#include <ti/net/slnetsock.h>
#include <ti/net/slnetif.h>
#include <ti/net/slneterr.h>
#include <ti/net/slnetutils.h>

/* NDK support */
#include <ti/ndk/inc/socketndk.h>
#include <ti/ndk/inc/os/osif.h>

#include <ti/display/Display.h>

#include <wifi_ethernet_sockets.h>

extern Display_Handle display;

/*
 *  ======== TCPSocket ========
 *  Thread to create a client and connect to Python server script,
 *  choosing the default interface as Ethernet, then using WiFi when
 *  Ethernet is disconnected
 */

void * TCPSocket(void *args)
{
    fdOpenSession(TaskSelf());

    int16_t WiFiState;
    int16_t EthernetState;
    int retc;
    int server;
    char txBuf[5] = "hello";
    char rxBuf[10];
    struct SlNetSock_AddrIn_t serverAddr;

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = SLNETSOCK_AF_INET;
    retc = SlNetUtil_inetPton(SLNETSOCK_AF_INET, IPADDRESS,
                              &(serverAddr.sin_addr));
    serverAddr.sin_port = SlNetUtil_htons(TCPPORT);

    Display_printf(display, 0, 0,
                   "Ethernet Interface connected and started\n");

    while(1)
    {
        WiFiState = SlNetIf_getConnectionStatus(SLNETIF_ID_1);
        EthernetState = SlNetIf_getConnectionStatus(SLNETIF_ID_2);

        if((WiFiState == SLNETERR_RET_CODE_INVALID_INPUT) &&
           (EthernetState == SLNETERR_RET_CODE_INVALID_INPUT))
        {
            Display_printf(display, 0, 0, "Interface(s) not added yet\n");
            sleep(5);
        }
        else if((WiFiState == 0) && (EthernetState == 0))
        {
            Display_printf(display, 0, 0, "Interface(s) are not connected\n");
            sleep(5);
        }
        else
        {
            server =
                SlNetSock_create(SLNETSOCK_AF_INET, SLNETSOCK_SOCK_STREAM,
                                 SLNETSOCK_PROTO_TCP,
                                 0, SLNETSOCK_CREATE_IF_STATUS_CONNECTED);

            if(server < 0)
            {
                Display_printf(display, 0, 0, "Socket failed\n");
                goto shutdown;
            }

            retc = SlNetSock_connect(server,
                                     (const SlNetSock_Addr_t *)&serverAddr,
                                     sizeof(serverAddr));

            if((retc = SlNetSock_send(server, txBuf, sizeof(txBuf), 0)) < 0)
            {
                Display_printf(display, 0, 0, "Send failed\n");
                goto shutdown;
            }

            if((retc = SlNetSock_recv(server, rxBuf, sizeof(rxBuf), 0)) < 0)
            {
                Display_printf(display, 0, 0, "Receive failed\n");
                goto shutdown;
            }

            rxBuf[retc] = '\0';
            Display_printf(display, 0, 0, "%s", rxBuf);

            SlNetSock_close(server);

            sleep(5);
        }
    }
shutdown:
    if(server != -1)
    {
        SlNetSock_close(server);
    }

    fdCloseSession(TaskSelf());

    return (NULL);
}
