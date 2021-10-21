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

/*
 *  ======== main_nortos.c ========
 */
#include <stdint.h>
#include <stdlib.h>

/* TI Drivers Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Power.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* NoRTOS Header file */
#include <NoRTOS.h>

/* Local Files */
#include "ti_drivers_config.h"
#include "ota_extended.h"

extern void mainThread(void *pvParameters);

/* Type Definitions */
typedef void (*function_ptr_t)(void);

#if defined(__TI_COMPILER_VERSION__)
#pragma RETAIN(appTransferKey)
#pragma LOCATION(appTransferKey, SHARED_MEMORY_LOCATION)
volatile uint32_t appTransferKey;
#elif defined(__GNUC__)
volatile uint32_t __attribute__((section (".shared_memory"))) appTransferKey;
#elif defined(__IAR_SYSTEMS_ICC__)
__no_init volatile uint32_t appTransferKey @ SHARED_MEMORY_LOCATION;
#endif
/*
 *  ======== main ========
 */
int main(void)
{
    uint32_t *ptrApp;
    function_ptr_t functionPtr;

    /* If the application is valid, set the program counter to the application
     * reset vector
     */
    if(appTransferKey != APPLICATION_PERFORM_BSL)
    {
        /* Jump to application reset vector */
        ptrApp = (uint32_t *)(APP_SW_START_ADDRESS + 4);
        functionPtr = (function_ptr_t)*ptrApp;
        functionPtr();
    }
    else
    {
        /* Clear transfer key */
        appTransferKey = 0;

        /* Call board init functions */
        Board_initGeneral();

        /* Start NoRTOS */
        NoRTOS_start();

        /* Start the main function */
        mainThread(NULL);

        /* Force Reset */
        MAP_SysCtl_A_rebootDevice();
    }

    return (0);
}
