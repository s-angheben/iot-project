/*
 * Copyright (c) 2017-2018, Texas Instruments Incorporated
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
/******************************************************************************
*     cc_pal.c
*
*    Simplelink Wi-Fi platform abstraction file for MSP432
******************************************************************************/
#include <ti/drivers/SPI.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/net/wifi/porting/cc_pal.h>
#include <ti/drivers/net/wifi/porting/SIMPLELINKWIFI.h>
#include <ti/drivers/net/wifi/simplelink.h>

/****************************************************************************
   GLOBAL VARIABLES
****************************************************************************/
volatile Fd_t g_SpiFd = 0;
SL_P_EVENT_HANDLER g_Host_irq_Hndlr = NULL;

/****************************************************************************
   CONFIGURATION VARIABLES
****************************************************************************/
extern const WiFi_Config WiFi_config[];
extern const uint_least8_t WiFi_count;
static SIMPLELINKWIFI_HWAttrsV1* curDeviceConfiguration;

/****************************************************************************
   CONFIGURATION FUNCTION DEFINITION
****************************************************************************/
void WiFi_init()
{
    /* We need to have at least one WiFi module. */
    if(WiFi_count == 0)
    {
        return;
    }

    curDeviceConfiguration = (SIMPLELINKWIFI_HWAttrsV1*) WiFi_config[0].hwAttrs;
}

/****************************************************************************
   LOCAL FUNCTION DEFINITIONS
****************************************************************************/
Fd_t spi_Open(char *ifName,
              unsigned long flags)
{
    void *lspi_hndl;
    SPI_Params SPI_Config;

    /* Initialize the WiFi driver */
    WiFi_init();

    /* If we could not initialize the device bail out with an error code */
    if(curDeviceConfiguration == NULL)
    {
        return (-1);
    }

    /* Initialize the SPI config structure */
    SPI_Params_init(&SPI_Config);
    SPI_Config.transferMode = SPI_MODE_BLOCKING;
    SPI_Config.mode = SPI_MASTER;
    SPI_Config.bitRate = curDeviceConfiguration->spiBitRate;
    SPI_Config.dataSize = 8;
    SPI_Config.frameFormat = SPI_POL0_PHA0;

    /* Open SPI interface with SPI_Config parameters */
    lspi_hndl = SPI_open(curDeviceConfiguration->spiIndex, &SPI_Config);

    if(NULL == lspi_hndl)
    {
        return (-1);
    }
    else
    {
        return ((Fd_t) lspi_hndl);
    }
}

int spi_Close(Fd_t fd)
{
    SPI_close((void *) fd);
    return (0);
}

int spi_Read(Fd_t fd,
             unsigned char *pBuff,
             int len)
{
    SPI_Transaction transact_details;
    int read_size = 0;

    ASSERT_CS();

    /* check if the link SPI has been initialized successfully */
    if(fd < 0)
    {
        DEASSERT_CS();
        return (-1);
    }

    transact_details.txBuf = NULL;
    transact_details.arg = NULL;
    transact_details.rxBuf = (void*) (pBuff);

    while(len > 0)
    {
        if(len > curDeviceConfiguration->maxDMASize)
        {
            transact_details.count = curDeviceConfiguration->maxDMASize;
        }
        else
        {
            transact_details.count = len;
        }

        if(SPI_transfer((SPI_Handle) fd, &transact_details))
        {
            read_size += transact_details.count;
            len = len - transact_details.count;
            transact_details.rxBuf = ((unsigned char *) (transact_details.rxBuf)
                                      + transact_details.count);
        }
        else
        {
            DEASSERT_CS();
            return (-1);
        }
    }

    DEASSERT_CS();

    return (read_size);
}

int spi_Write(Fd_t fd,
              unsigned char *pBuff,
              int len)
{
    SPI_Transaction transact_details;
    int write_size = 0;

    ASSERT_CS();

    /* check if the link SPI has been initialized successfully */
    if(fd < 0)
    {
        DEASSERT_CS();
        return (-1);
    }

    transact_details.rxBuf = NULL;
    transact_details.arg = NULL;
    transact_details.txBuf = (void*) (pBuff);

    while(len > 0)
    {
        if(len > curDeviceConfiguration->maxDMASize)
        {
            transact_details.count = curDeviceConfiguration->maxDMASize;
        }
        else
        {
            transact_details.count = len;
        }

        if(SPI_transfer((SPI_Handle) fd, &transact_details))
        {
            write_size += transact_details.count;
            len = len - transact_details.count;
            transact_details.txBuf = ((unsigned char *) (transact_details.txBuf)
                                      + transact_details.count);
        }
        else
        {
            DEASSERT_CS();
            return (-1);
        }
    }

    DEASSERT_CS();

    return (write_size);
}

int NwpRegisterInterruptHandler(P_EVENT_HANDLER InterruptHdl,
                                void* pValue)
{
    /* Check for unregister condition */
    if(NULL == InterruptHdl)
    {
        GPIO_disableInt(curDeviceConfiguration->hostIRQPin);
        GPIO_clearInt(curDeviceConfiguration->hostIRQPin);
        g_Host_irq_Hndlr = NULL;
        return (0);
    }
    else if(NULL == g_Host_irq_Hndlr)
    {
        g_Host_irq_Hndlr = InterruptHdl;
        GPIO_setCallback(curDeviceConfiguration->hostIRQPin,
                         HostIrqGPIO_callback);
        GPIO_clearInt(curDeviceConfiguration->hostIRQPin);
        GPIO_enableInt(curDeviceConfiguration->hostIRQPin);
        return (0);
    }
    else
    {
        /* An error occurred */
        return (-1);
    }
}

void HostIrqGPIO_callback(uint_least8_t index)
{
    if((index == curDeviceConfiguration->hostIRQPin)
       && (NULL != g_Host_irq_Hndlr))
    {
        g_Host_irq_Hndlr(0);
    }
}

void NwpMaskInterrupt()
{
}

void NwpUnMaskInterrupt()
{
}

void NwpPowerOnPreamble(void)
{
    /* Maybe start timer here? */
}

void NwpPowerOn(void)
{
    GPIO_write(curDeviceConfiguration->nHIBPin, 1);
    /* wait 5msec */
    ClockP_usleep(5000);
}

void NwpPowerOff(void)
{
    GPIO_write(curDeviceConfiguration->nHIBPin, 0);
    /* wait 5msec */
    ClockP_usleep(5000);
}

#if defined(SL_PLATFORM_MULTI_THREADED)

int Semaphore_pend_handle(sem_t* pSemHandle,
                          uint32_t timeout)
{
    if(OS_WAIT_FOREVER == timeout)
    {
        return(sem_wait(pSemHandle));
    }
    else
    {
        struct timespec abstime;
        abstime.tv_nsec = 0;
        abstime.tv_sec = 0;

        /* Since POSIX timeout are relative and not absolute,
         * take the current timestamp. */
        clock_gettime(CLOCK_REALTIME, &abstime);
        if(abstime.tv_nsec < 0)
        {
            abstime.tv_sec = timeout;
            return (sem_timedwait(pSemHandle, &abstime));
        }

        /* Add the amount of time to wait */
        abstime.tv_sec += timeout / 1000;
        abstime.tv_nsec += (timeout % 1000) * 1000000;

        abstime.tv_sec += (abstime.tv_nsec / 1000000000);
        abstime.tv_nsec = abstime.tv_nsec % 1000000000;

        /* Call the semaphore wait API */
        return(sem_timedwait(pSemHandle, &abstime));
    }
}

int Mutex_create_handle(pthread_mutex_t *pMutexHandle)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    if(pthread_mutex_init(pMutexHandle, &attr) < 0)
    {
        return(Mutex_FAILURE);
    }

    return(Mutex_OK);
}

#else

int SemaphoreP_create_handle(SemaphoreP_Handle* pSemHandle)
{
    SemaphoreP_Params params;

    SemaphoreP_Params_init(&params);

    params.callback = tiDriverSpawnCallback;

    (*(pSemHandle)) = SemaphoreP_create(1, &params);

    if(!(*(pSemHandle)))
    {
        return(-1);
    }

    return(SemaphoreP_OK);
}

int SemaphoreP_delete_handle(SemaphoreP_Handle* pSemHandle)
{
    SemaphoreP_delete(*(pSemHandle));
    return(SemaphoreP_OK);
}

int SemaphoreP_post_handle(SemaphoreP_Handle* pSemHandle)
{
    SemaphoreP_post(*(pSemHandle));
    return(SemaphoreP_OK);
}

int Mutex_create_handle(MutexP_Handle* pMutexHandle)
{
    MutexP_Params params;

    MutexP_Params_init(&params);

    params.callback = tiDriverSpawnCallback;

    (*(pMutexHandle)) = MutexP_create(&params);

    if(!(*(pMutexHandle)))
    {
        return(Mutex_FAILURE);
    }

    return(MutexP_OK);
}

int MutexP_delete_handle(MutexP_Handle* pMutexHandle)
{
    MutexP_delete(*(pMutexHandle));
    return (MutexP_OK);
}

int Mutex_unlock(MutexP_Handle pMutexHandle)
{
    MutexP_unlock(pMutexHandle, 0);
    return (MutexP_OK);
}

int Mutex_lock(MutexP_Handle pMutexHandle)
{
    MutexP_lock(pMutexHandle);
    return (MutexP_OK);
}

#endif

unsigned long TimerGetCurrentTimestamp()
{
    return (ClockP_getSystemTicks());
}
