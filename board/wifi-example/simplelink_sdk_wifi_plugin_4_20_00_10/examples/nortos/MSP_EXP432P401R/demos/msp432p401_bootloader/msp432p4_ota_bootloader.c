/* Standard includes */
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

/* TI-DRIVERS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Local Includes */
#include "ota_extended.h"
#include "ti_drivers_config.h"

//*****************************************************************************
// Statics
//*****************************************************************************
static void errorLoop();

//*****************************************************************************
// SimpleLink Callback Functions
//*****************************************************************************
//*****************************************************************************
//
//! \brief The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************

void SimpleLinkSocketTriggerEventHandler(SlSockTriggerEvent_t *pSlTriggerEvent)
{
    /* Do Nothing */
}

void SimpleLinkNetAppRequestMemFreeEventHandler(uint8_t *buffer)
{
    /* Do Nothing */
}

void SimpleLinkNetAppRequestEventHandler(SlNetAppRequest_t *pNetAppRequest,
                                         SlNetAppResponse_t *pNetAppResponse)
{
    /* Do Nothing */
}

//*****************************************************************************
//
//! \brief The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    /* Do Nothing */
}

//*****************************************************************************
//
//! \brief The Function Handles the Fatal errors
//!
//! \param[in]  slFatalErrorEvent - Pointer to Fatal Error Event info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkFatalErrorEventHandler(SlDeviceFatal_t *slFatalErrorEvent)
{
    /* Do Nothing */
}

//*****************************************************************************
//
//! \brief This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    /* Do Nothing */
}

//*****************************************************************************
//
//! \brief This function handles HTTP server events
//!
//! \param[in]  pServerEvent - Contains the relevant event information
//! \param[in]    pServerResponse - Should be filled by the user with the
//!                                      relevant response information
//!
//! \return None
//!
//****************************************************************************
void SimpleLinkHttpServerEventHandler(SlNetAppHttpServerEvent_t *pHttpEvent,
                                      SlNetAppHttpServerResponse_t * pHttpResponse)
{
    /* Do Nothing */
}

//*****************************************************************************
//
//! \brief This function handles General Events
//!
//! \param[in]     pDevEvent - Pointer to General Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    /* Do Nothing */
}

//*****************************************************************************
//
//! This function handles socket events indication
//!
//! \param[in]      pSock - Pointer to Socket Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    /* Do Nothing */
}

void mainThread(void *pvParameters)
{
    int32_t devHandle;
    SlFsFileInfo_t pFsFileInfo;
    int32_t resCode;
    uint32_t bytesRead = 0;
    uint32_t bytesLeft;
    uint32_t jj;
    uint32_t halfAddress;
    uint32_t curSector;
    uint32_t sectorMask = 0;
    uint32_t maxFlashAddress = MAP_SysCtl_getFlashSize() / 2;
    uint8_t transferBuffer[OTA_BUFFER_SIZE];
    SlFsControlGetStorageInfoResponse_t SlFsControlGetStorageInfoResponse;
    SlFsControl_t FsControl;

    SPI_init();
    GPIO_init();

    /* Turning on the blue LED to signify OTA status */
    GPIO_write(MSP_EXP432P401R_GPIO_LED_BLUE, Board_GPIO_LED_ON);

    /* Starting the NWP */
    resCode = sl_Start(0, 0, 0);

    if(resCode < 0)
    {
        errorLoop();
    }

    /* The SimpleLink host driver architecture mandate calling 'sl_task' in
     * a NO-RTOS application's main loop. The purpose of this call, is to
     * handle asynchronous events and get flow control information sent
     * from the NWP. Every event is classified and later handled by the
     * host driver event handlers.
     */
    sl_Task(NULL);

    /* Check to see if there is a bundle write pending. If there is,
     * commit the bundle write
     */
    resCode = (int16_t) sl_FsCtl((SlFsCtl_e) SL_FS_CTL_GET_STORAGE_INFO, 0,
                                 NULL,
                                 NULL, 0,
                                 (uint8_t *) &SlFsControlGetStorageInfoResponse,
                                 sizeof(SlFsControlGetStorageInfoResponse_t),
                                 NULL);
    if(resCode < 0)
    {
        errorLoop();
    }
    else
    {
        FsControl.IncludeFilters = 0;
        resCode = (int16_t) sl_FsCtl(SL_FS_CTL_BUNDLE_COMMIT, 0, NULL,
                                     (uint8_t *) &FsControl,
                                     sizeof(SlFsControl_t),
                                     NULL,
                                     0,
                                     NULL);
        if(resCode < 0)
        {
            errorLoop();
        }
    }

    if((SlFsBundleState_e) SlFsControlGetStorageInfoResponse.FilesUsage.
       Bundlestate
       == SL_FS_BUNDLE_STATE_PENDING_COMMIT)
    {
    }

    /* Seeing if the firmware image exists in the new image  */
    resCode = sl_FsGetInfo(OTA_FIRMWARE_PATH, 0, &pFsFileInfo);

    if(resCode < 0)
    {
        errorLoop();
    }

    /* Opening the firmware image and writing to the flash in chunks */
    devHandle = sl_FsOpen(OTA_FIRMWARE_PATH, SL_FS_READ, 0);

    if(devHandle < 0)
    {
        errorLoop();
    }

    /* Going through and unprotecting all of the sectors that we want to
     * erase.
     */
    for(jj = APP_SW_START_ADDRESS;
        jj < (pFsFileInfo.Len + APP_SW_START_ADDRESS); jj += 4096)
    {
        /* If we crossed the bank boundaries we have to unprotect the
         * sectors in the current bank and then recalculate for the new
         * bank
         */
        if(jj >= maxFlashAddress)
        {
            break;
        }

        /* Calculating the current sector and adding it to the mask */
        curSector = (jj / 4096);
        curSector = 1 << curSector;
        sectorMask |= curSector;
    }

    /* Unprotecting the sector mask in Bank 0 */
    MAP_FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK0, sectorMask);

    if(jj == maxFlashAddress)
    {
        sectorMask = 0;
        curSector = 0;
        jj = maxFlashAddress;
        maxFlashAddress = MAP_SysCtl_getFlashSize();
        halfAddress = maxFlashAddress / 2;

        /* Unprotecting the rest of the sectors */
        for(; (jj - APP_SW_START_ADDRESS) < (pFsFileInfo.Len); jj += 4096)
        {
            /* If the current address is out of main memory (or bank 0)
             *  cancel out */
            if(jj >= maxFlashAddress)
            {
                errorLoop();
            }

            /* Calculating the current sector and adding it to the mask */
            curSector = ((jj - halfAddress) / 4096);
            curSector = 1 << curSector;
            sectorMask |= curSector;
        }

        /* Unprotecting the sector mask in Bank 1 */
        MAP_FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK1, sectorMask);
    }

    /* Erasing all unprotected sectors */
    if(!MAP_FlashCtl_performMassErase())
    {
        errorLoop();
    }

    /* Writing the new image */
    bytesLeft = pFsFileInfo.Len;

    while(bytesLeft != 0)
    {
        if(bytesLeft < OTA_BUFFER_SIZE)
        {
            resCode = sl_FsRead(devHandle, bytesRead,
                                (unsigned char *) transferBuffer, bytesLeft);
        }
        else
        {
            resCode = sl_FsRead(devHandle, bytesRead,
                                (unsigned char *) transferBuffer,
                                OTA_BUFFER_SIZE);
        }

        if(resCode < 0)
        {
            errorLoop();
        }

        if(!ROM_FlashCtl_programMemory(
               transferBuffer, (void*) (APP_SW_START_ADDRESS + bytesRead),
               resCode))
        {
            errorLoop();
        }

        bytesRead += resCode;
        bytesLeft -= resCode;
    }

    if(bytesRead != pFsFileInfo.Len)
    {
        errorLoop();
    }

    sl_FsClose(devHandle, NULL, NULL, 0);

    sl_Stop(0);

    GPIO_write(MSP_EXP432P401R_GPIO_LED_BLUE, Board_GPIO_LED_OFF);
}

static void errorLoop()
{
    uint32_t ii;

    while(1)
    {
        for(ii = 0; ii < 500000; ii++)
        {
            ;
        }
        GPIO_toggle(MSP_EXP432P401R_GPIO_LED_RED);
        GPIO_toggle(MSP_EXP432P401R_GPIO_LED_GREEN);
        GPIO_toggle(MSP_EXP432P401R_GPIO_LED_BLUE);
    }
}
