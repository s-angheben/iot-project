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

#ifndef __PLATFORM_H
#define __PLATFORM_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
// Includes
//*****************************************************************************
#ifndef NORTOS_SUPPORT
#include <pthread.h>
#endif
#include <time.h>

//*****************************************************************************
// Defines
//*****************************************************************************
#ifdef NORTOS_SUPPORT
#ifndef sigval
#define sigval int
#endif
#endif

extern int Report(const char *pcFormat,
                  ...);
#define UART_PRINT  Report

//*****************************************************************************
// Typedefs
//*****************************************************************************
typedef struct
{
    uint32_t TSPrev;
    uint32_t TSCurr;
    uint32_t DeltaTicks;
    uint32_t DeltaTicksReminder;
    int32_t Total10MSecUnits;
} PlatformTimeout_t;

typedef struct e_Timers
{
    unsigned long ePeripheral;
    unsigned long base;
    unsigned long timer;
} e_Timers;

typedef struct sBootInfo
{
    uint8_t ucActiveImg;
    uint32_t ulImgStatus;
    uint32_t ulStartWdtKey;
    uint32_t ulStartWdtTime;
} sBootInfo_t;

//*****************************************************************************
// Function prototypes
//*****************************************************************************
extern void Platform_Init();
extern void Platform_Reset();
extern int32_t Platform_CommitWdtConfig(int32_t TimeoutInSeconds);
extern void Platform_CommitWdtStop();
extern void Platform_TimerInit(void (*timerIntHandler)(sigval val));
extern void Platform_TimerStart(uint32_t asyncEvtTimeoutMsec);
extern void Platform_TimerStop();
extern void Platform_TimerInterruptClear();
extern void Platform_TimeoutStart(PlatformTimeout_t *pTimeoutInfo,
                                  uint32_t TimeoutInMsec);
extern uint8_t Platform_TimeoutIsExpired(PlatformTimeout_t *pTimeoutInfo);
extern uint32_t Platform_MsecTimestamp();
extern void Platform_Sleep(uint32_t mSecDuration);
extern void Platform_LedToggle();
extern void Platform_FactoryDefaultIndication();
extern void Platform_gpioButtonFxn0(unsigned int index);
extern void Platform_processOTAImage(void);
extern void notifyOtaCommandArrived();

#ifdef __cplusplus
}
#endif

#endif /* __PLATFORM_H */
