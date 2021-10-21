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
#ifndef ETHERNET_WIFI_TCP_ECHO_H_
#define ETHERNET_WIFI_TCP_ECHO_H_

/* Number of TCP clients that can connect to the TCP server */
#define NUMTCPWORKERS            3

/* TCP Port that clients should connect to */
#define TCPPORT                  1000

/* Maximum size of TCP packet to receive from clients */
#define TCPPACKETSIZE            256

/* Stack size in bytes */
#define HANDLERSTACKSIZE         2048

/* Stack size for Ethernet/WiFi writer threads */
#define WRITERSTACKSIZE          2048

/* Stack size for Ethernet/WiFi writer threads */
#define READERSTACKSIZE          2048

/* Stack size for threads listening on TCP port */
#define TCPHANDLERSTACKSIZE      2048

typedef struct
{
    int clientfd;
    int flag;
} ReaderData;

/*
 *  ======== EthernetWiFiTCPEchoInit ========
 *  Initialization function to initialize the data structures used for this
 *  demo. Should be called before the RTOS starts.
 */
void EthernetWiFiTCPEchoInit(void);

/*
 *  ======== WriterThread ========
 *  Thread that writes new data to any sockets open over WiFi/Ethernet
 * interface
 */
void * WriterThread(void *threadArgs);

/*
 *  ======== ReaderThread ========
 *  Task to handle TCP connection. Can be multiple Tasks running
 *  this function.
 */
void * ReaderThread(void *threadArgs);

#endif /* ETHERNET_WIFI_TCP_ECHO_H_ */
