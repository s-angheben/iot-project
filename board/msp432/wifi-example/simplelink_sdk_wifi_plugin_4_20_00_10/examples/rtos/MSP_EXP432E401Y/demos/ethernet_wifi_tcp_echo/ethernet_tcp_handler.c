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

/* SlNetSock support */
#include <ti/net/slnetsock.h>
#include <ti/net/slnetutils.h>
#include <ti/net/slnetif.h>

/* NDK support */
#include <ti/ndk/inc/socketndk.h>
#include <ti/ndk/inc/os/osif.h>

#include <ti/display/Display.h>

#include "ethernet_wifi_tcp_echo.h"

extern Display_Handle display;

/*
 *  ======== EthernetTCPHandler ========
 *  Thread to bind to socket and start new threads when clients connect to
 *  open socket
 */
void * EthernetTCPHandler(void *args)
{
    fdOpenSession(TaskSelf());

    ReaderData rData;
    rData.flag = SLNETIF_ID_2;
    int retc;
    int status;
    int server;
    int flag = SLNETIF_ID_2;
    struct SlNetSock_AddrIn_t localAddr;
    struct SlNetSock_AddrIn_t clientAddr;
    int optval;
    int optlen = sizeof(optval);
    SlNetSocklen_t addrlen = sizeof(clientAddr);

    pthread_t reader_thread = (pthread_t) NULL;
    pthread_attr_t pAttrs_reader;
    struct sched_param readerParam;

    pthread_t writer_thread = (pthread_t) NULL;
    pthread_attr_t pAttrs_writer;
    struct sched_param writerParam;

    /* Starting up writer thread */
    pthread_attr_init(&pAttrs_writer);
    writerParam.sched_priority = 3;
    retc = pthread_attr_setschedparam(&pAttrs_writer, &writerParam);
    retc |= pthread_attr_setstacksize(&pAttrs_writer, WRITERSTACKSIZE);
    retc |= pthread_create(&writer_thread, &pAttrs_writer, &WriterThread,
                           &flag);

    Display_printf(display, 0, 0, "Ethernet Interface connected and started\n");

    server =
        SlNetSock_create(SLNETSOCK_AF_INET, SLNETSOCK_SOCK_STREAM,
                         SLNETSOCK_PROTO_TCP,
                         SLNETIF_ID_2,
                         SLNETSOCK_CREATE_IF_STATE_ENABLE);
    if(server == -1)
    {
        Display_printf(display, 0, 0, "Ethernet: socket failed\n");
        goto shutdown;
    }

    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = SLNETSOCK_AF_INET;
    localAddr.sin_addr.s_addr = SlNetUtil_htonl(SLNETSOCK_INADDR_ANY);
    localAddr.sin_port = SlNetUtil_htons(TCPPORT);

    status = SlNetSock_bind(server, (const SlNetSock_Addr_t *)&localAddr,
                            sizeof(localAddr));
    if(status == -1)
    {
        Display_printf(display, 0, 0, "Ethernet: bind failed\n");
        goto shutdown;
    }

    status = SlNetSock_listen(server, NUMTCPWORKERS);

    if(status == -1)
    {
        Display_printf(display, 0, 0, "Ethernet: listen failed\n");
        goto shutdown;
    }

    optval = 1;
    status =
        SlNetSock_setOpt(server, SLNETSOCK_LVL_SOCKET,
                         SLNETSOCK_OPSOCK_KEEPALIVE,
                         &optval,
                         optlen);
    if(status == -1)
    {
        Display_printf(display, 0, 0, "Ethernet: setsockopt failed\n");
        goto shutdown;
    }

    while((rData.clientfd =
               SlNetSock_accept(server, (SlNetSock_Addr_t *)&clientAddr,
                                &addrlen)) != -1)
    {
        Display_printf(display, 0, 0,
                       "Ethernet: Creating thread clientfd = %x\n",
                       rData.clientfd);

        pthread_attr_init(&pAttrs_reader);
        readerParam.sched_priority = 3;
        retc = pthread_attr_setschedparam(&pAttrs_reader, &readerParam);
        retc |= pthread_attr_setstacksize(&pAttrs_reader, READERSTACKSIZE);

        retc |=
            pthread_create(&reader_thread, &pAttrs_reader, &ReaderThread,
                           &rData);

        if(retc != 0)
        {
            Display_printf(display, 0, 0,
                           "Ethernet: Error - Failed to create new thread.\n");
            SlNetSock_close(rData.clientfd);
        }

        /* addrlen is a value-result param, must reset for next accept call */
        addrlen = sizeof(clientAddr);
    }

    Display_printf(display, 0, 0, "Ethernet: accept failed.\n");

    fdCloseSession(TaskSelf());

shutdown:
    if(server != -1)
    {
        SlNetSock_close(server);
    }

    return (NULL);
}
