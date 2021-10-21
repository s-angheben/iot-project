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

/* POSIX Header files */
#include <pthread.h>
#include <mqueue.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>

/* TI-Driver includes */
#include <ti/display/Display.h>

/* SlNetSock support */
#include <ti/net/slnetsock.h>
#include <ti/net/slnetif.h>

/* NDK support */
#include <ti/ndk/inc/socketndk.h>
#include <ti/ndk/inc/os/osif.h>

/* Application includes */
#include "ethernet_wifi_tcp_echo.h"

Display_Handle display;

extern void ti_ndk_config_Global_startupFxn();
extern void ti_simplelink_host_config_Global_startupFxn();

/*
 * Internal data structure used to pass data along message queues
 */
typedef struct
{
    char buffer[TCPPACKETSIZE];
    int size;
} QueueMsg;

/*
 * Queues to keep track of messages recieved from the network interfaces
 */
mqd_t EthernetMsgs;
mqd_t WiFiMsgs;

/*
 * Arrays to keep track of client socket file descriptors
 */
int EthernetClientFD[NUMTCPWORKERS];
int WiFiClientFD[NUMTCPWORKERS];

/*
 * Mutexes to lock the client socket file descriptors arrays
 */
pthread_mutex_t TCPWorkerEthernetFDMutex;
pthread_mutex_t TCPWorkerWiFiFDMutex;

/*
 *  ======== EthernetWiFiTCPEchoInit ========
 *  Initialization function to initialize the data structures used for this
 *  demo. Should be called before the RTOS starts.
 */
void EthernetWiFiTCPEchoInit(void) {
    int i = 0;
    struct mq_attr attr;

    /* Reset Client File Descriptors to -1 */
    pthread_mutex_init(&TCPWorkerEthernetFDMutex, NULL);
    for(i = 0; i < NUMTCPWORKERS; i++)
    {
        EthernetClientFD[i] = -1;
    }

    /* Reset Client File Descriptors to -1 */
    pthread_mutex_init(&TCPWorkerWiFiFDMutex, NULL);
    for(i = 0; i < NUMTCPWORKERS; i++)
    {
        WiFiClientFD[i] = -1;
    }

    /* Create message queues to pass data between interfaces */
    attr.mq_maxmsg = NUMTCPWORKERS;
    attr.mq_msgsize = sizeof(QueueMsg);
    attr.mq_flags = 0;

    EthernetMsgs = mq_open("EthernetQueue", O_RDWR | O_CREAT, 0644, &attr);
    if(EthernetMsgs == (mqd_t)(-1))
    {
        while(1)
        {
            ;
        }
    }

    WiFiMsgs = mq_open("WiFiQueue", O_RDWR | O_CREAT, 0644, &attr);
    if(WiFiMsgs == (mqd_t)(-1))
    {
        while(1)
        {
            ;
        }
    }
}

/*
 *  ======== NewTCPWorker ========
 *  Add the client file descriptor from the active interface list once it
 * connects
 */
static void NewTCPWorker(int fd,
                         pthread_mutex_t *mutex,
                         int *clientFD) {
    int i = 0;
    pthread_mutex_lock(mutex);
    for(i = 0; i < NUMTCPWORKERS; i++)
    {
        if(clientFD[i] == -1)
        {
            clientFD[i] = fd;
            break;
        }
    }
    pthread_mutex_unlock(mutex);
}

/*
 *  ======== removeTcpWorker ========
 *  Remove the client file descriptor from the active interface list once it
 * disconnects
 */
static void RemoveTCPWorker(int fd,
                            pthread_mutex_t *mutex,
                            int *clientFD) {
    int i = 0;
    pthread_mutex_lock(mutex);
    for(i = 0; i < NUMTCPWORKERS; i++)
    {
        if(clientFD[i] == fd)
        {
            clientFD[i] = -1;
            break;
        }
    }
    pthread_mutex_unlock(mutex);
}

/*
 *  ======== WriterThread ========
 *  Thread that writes new data to any sockets open over WiFi/Ethernet
 * interface
 */
void * WriterThread(void *threadArgs)
{
    int flag = *((int *) threadArgs);
    int bytesSent;
    int bytesRcvd;
    int i;
    int *clientFD;
    mqd_t *messages;
    pthread_mutex_t *TCPWorkerMutex;

    QueueMsg msg;

    /* Ethernet ID */
    if(flag == SLNETIF_ID_2)
    {
        TCPWorkerMutex = &TCPWorkerEthernetFDMutex;
        messages = &WiFiMsgs;
        clientFD = EthernetClientFD;
        fdOpenSession(TaskSelf());
    }
    /* WiFi ID*/
    else
    {
        TCPWorkerMutex = &TCPWorkerWiFiFDMutex;
        messages = &EthernetMsgs;
        clientFD = WiFiClientFD;
    }

    while(1)
    {
        bytesRcvd = mq_receive(*messages, (char *)(&msg), sizeof(QueueMsg), 0);
        if(bytesRcvd != -1)
        {
            pthread_mutex_lock(TCPWorkerMutex);
            for(i = 0; i < NUMTCPWORKERS; i++)
            {
                if(clientFD[i] != -1)
                {
                    bytesSent =
                        SlNetSock_send(clientFD[i], msg.buffer, msg.size,
                                       0);
                    if(bytesSent < 0 || bytesSent != msg.size)
                    {
                        Display_printf(display, 0, 0, "send failed.\n");
                        break;
                    }
                }
            }
            pthread_mutex_unlock(TCPWorkerMutex);
        }
        else
        {
            Display_printf(display, 0, 0, "mq receive failed.\n");
            break;
        }
    }

    /* Ethernet ID */
    if(flag == SLNETIF_ID_2)
    {
        fdCloseSession(TaskSelf());
    }

    return (NULL);
}

/*
 *  ======== ReaderThread ========
 *  Task to handle Ethernet/WiFi TCP connection. Can be multiple Tasks running
 *  this function.
 */
void * ReaderThread(void *threadArgs)
{
    int clientfd = (*((ReaderData *) threadArgs)).clientfd;
    int flag = (*((ReaderData *) threadArgs)).flag;

    int retc;
    int *clientFD;
    mqd_t *messages;
    pthread_mutex_t *TCPWorkerMutex;
    const char *interfaceStr = SlNetIf_getNameByID(flag);
    QueueMsg msg;

    /* Ethernet ID */
    if(flag == SLNETIF_ID_2)
    {
        TCPWorkerMutex = &TCPWorkerEthernetFDMutex;
        messages = &EthernetMsgs;
        clientFD = EthernetClientFD;
        fdOpenSession(TaskSelf());
    }
    else
    {
        /* WiFi ID */
        TCPWorkerMutex = &TCPWorkerWiFiFDMutex;
        messages = &WiFiMsgs;
        clientFD = WiFiClientFD;
    }

    NewTCPWorker(clientfd, TCPWorkerMutex, clientFD);

    Display_printf(display, 0, 0, "%s: start clientfd = 0x%x\n", interfaceStr,
                   clientfd);

    while((msg.size =
               SlNetSock_recv(clientfd, msg.buffer, TCPPACKETSIZE, 0)) > 0)
    {
        retc = mq_send(*messages, (char *)(&msg), sizeof(QueueMsg), 0);
        if(retc)
        {
            Display_printf(display, 0, 0, "send failed.\n");
            break;
        }
    }

    Display_printf(display, 0, 0, "%s: stop clientfd = 0x%x\n", interfaceStr,
                   clientfd);

    RemoveTCPWorker(clientfd, TCPWorkerMutex, clientFD);
    SlNetSock_close(clientfd);

    /* Ethernet ID */
    if(flag == SLNETIF_ID_2)
    {
        fdCloseSession(TaskSelf());
    }

    return (NULL);
}

/*
 *  ======== mainThread ========
 *  Thread that initializes the hardware and starts up the NDK and SimpleLink
 *  host stacks.
 */
void * mainThread(void *pvParameters)
{
    Display_init();
    GPIO_init();
    SPI_init();

    display = Display_open(Display_Type_UART, NULL);
    if(display == NULL)
    {
        /* Failed to open display driver */
        while(1)
        {
            ;
        }
    }

    EthernetWiFiTCPEchoInit();

    ti_simplelink_host_config_Global_startupFxn();
    ti_ndk_config_Global_startupFxn();

    return (0);
}
