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

/*****************************************************************************

   Application Name     - Power Measurement
   Application Overview - Power Measurement application enables the user to
                        measure current values, power consumption
                        and other such parameters for CC3220, when the device
                        practising different Low power modes. The other main
                        objective behind this application is to introduce the
                        user to the easily configurable power management drivers.

   Application Details  - Refer to 'Power Measurement' README.html

*****************************************************************************/

//****************************************************************************
//
//! \addtogroup
//! @{
//
//****************************************************************************

//*****************************************************************************
// Includes
//*****************************************************************************

// Standard includes
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

/* TI-DRIVERS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/net/wifi/simplelink.h>

#include "common.h"
#include "platform.h"
#include "uart_term.h"
#include "time.h"

//*****************************************************************************
// Defines
//*****************************************************************************

#define APPLICATION_NAME            "Power Measurement"
#define APPLICATION_VERSION         "1.0.0.2"

#define FRAME_LENGTH                (1000)
#define OPEN_SOCK_ONCE              (-2)
#define ALWAYS_OPEN_SOCK            (-1)
#define STATIC_IP                   (0)
#define DHCP_NO_FAST_RENEW          (1)
#define DHCP_FAST_RENEW_NO_ACK      (2)
#define DHCP_FAST_RENEW_WAIT_ACK    (3)

/* USER's defines */
#define PM_USECASE                  UseCase_AlwaysConnected /* options-> UseCase_HIB, UseCase_LPDS, UseCase_Transceiver,
                                                               UseCase_IntermittentlyConnected, UseCase_AlwaysConnected */
#define SOCKET_TYPE                 SocketType_UDP /* options -> SocketType_UDP , SocketType_TCP, SocketType_SEC_TCP */
#define PORT                        (5001)
#define DEST_IP_ADDR                SL_IPV4_VAL(192,168,39,200)
#define SRC_IP_ADDR                 SL_IPV4_VAL(192,168,39,1)        /* relevant for Static IP mode */
#define GATEWAY_IP_ADDR             SL_IPV4_VAL(192,168,39,242)        /* relevant for Static IP mode */
#define NUM_OF_PKT                  (1)
#define IP_ADDR_ALLOC_MODE          DHCP_FAST_RENEW_NO_ACK    /* options -> STATIC_IP, DHCP_NO_FAST_RENEW,
                                                                 DHCP_FAST_RENEW_NO_ACK, DHCP_FAST_RENEW_WAIT_ACK */
#define NOT_ACTIVE_DURATION_MSEC    (5000)  /* 5 seconds */
#define LPDS_IDLE_TIME              (5000)  /* 5 seconds */
#define LSI_DURATION_IN_MSEC        (100)
/* Tag setting defines */
#define CCA_BYPASS                  (1)                            /* Ignore the clear-channel-assessment indication */
#define TAG_FRAME_TRANSMIT_RATE     (6)
#define TAG_FRAME_TRANSMIT_POWER    (7)
#define TAG_CHANNEL                 (1)

#define SL_STOP_TIMEOUT             (200)

//*****************************************************************************
// Typedefs
//*****************************************************************************

typedef enum
{
    UseCase_HIB,
    UseCase_LPDS,
    UseCase_Transceiver,
    UseCase_IntermittentlyConnected,
    UseCase_AlwaysConnected
}UseCases;

typedef enum
{
    SocketType_UDP,
    SocketType_TCP,
    SocketType_SEC_TCP
}SocketTypes;

typedef struct _PowerMeasure_AppData_t_
{
    UseCases useCase;                 /* The exercised use case  */
    uint32_t pktsToDo;                /* how many packet to transmit on each interval of this use case */
    int32_t sockID;                   /* socket ID*/
    SocketTypes socketType;           /* Socket type */
    uint32_t ipAddr;                  /* IP address */
    uint32_t port;                    /* socket port number */
}PowerMeasure_AppData;

/* Control block definition */
typedef struct _PowerMeasure_ControlBlock_t_
{
    uint32_t slStatus;           //SimpleLink Status
    signed char frameData[FRAME_LENGTH];
    SlSockAddrIn_t ipV4Addr;
}PowerMeasure_ControlBlock;

//*****************************************************************************
// Function prototypes
//*****************************************************************************
void powerMeasureDoNothing();
int32_t displayBanner(void);
void startMeasureBanner(void);
int32_t wlanConnect(void);
void switchToStaMode(int32_t mode);
void setUseCase(void);
int32_t configSimplelinkToUseCase(void);
int32_t setIpAddrAllocMode(uint8_t mode);
int32_t transceiverMode(void);
int32_t intermittentlyConnected(void);
int32_t alwaysConnected(void);
int32_t bsdUdpClient(uint16_t Port,
                     int16_t Sid);
int32_t bsdTcpClient(uint16_t Port,
                     int16_t Sid);
int32_t bsdTcpSecClient(uint16_t Port,
                        int16_t Sid);
void prepareDataFrame(uint16_t Port,
                      uint32_t IpAddr);

//*****************************************************************************
// Globals
//*****************************************************************************
PowerMeasure_ControlBlock PowerMeasure_CB;

PowerMeasure_AppData PowerMeasure_appData = {
    PM_USECASE,              /* The exercised use case  */
    NUM_OF_PKT,              /* how many Intervals do on this use case */
    OPEN_SOCK_ONCE,          /* socket ID*/
    SOCKET_TYPE,             /* Socket type */
    DEST_IP_ADDR,            /* IP address */
    PORT,                    /* socket port number */
};

ClockP_Handle PowerMeasure_lpdsHandle;
//*****************************************************************************
// Callback Functions
//*****************************************************************************

//*****************************************************************************
//
//! \brief Callback function for the GPIO interrupt on Board_BUTTON1
//!
//! \param  index
//!
//! \return none
//!
//*****************************************************************************
void gpioButtonFxn1(uint32_t index)
{
}

//*****************************************************************************
// SimpleLink Callback Functions
//*****************************************************************************

void SimpleLinkNetAppRequestMemFreeEventHandler(uint8_t *buffer)
{
    // do nothing...
}

void SimpleLinkNetAppRequestEventHandler(SlNetAppRequest_t *pNetAppRequest,
                                         SlNetAppResponse_t *pNetAppResponse)
{
    // do nothing...
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
    switch(pWlanEvent->Id)
    {
    case SL_WLAN_EVENT_CONNECT:
    {
        SET_STATUS_BIT(PowerMeasure_CB.slStatus, STATUS_BIT_CONNECTION);
    }
    break;

    case SL_WLAN_EVENT_DISCONNECT:
    {
        CLR_STATUS_BIT(PowerMeasure_CB.slStatus, STATUS_BIT_CONNECTION);
        CLR_STATUS_BIT(PowerMeasure_CB.slStatus, STATUS_BIT_IP_ACQUIRED);
    }
    break;

    default:
    {
        UART_PRINT("[WLAN EVENT] Unexpected event [0x%x]\n\r",
                   pWlanEvent->Id);
    }
    break;
    }
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
    switch(slFatalErrorEvent->Id)
    {
    case SL_DEVICE_EVENT_FATAL_DEVICE_ABORT:
    {
        UART_PRINT(
            "[ERROR] - FATAL ERROR: Abort NWP event detected: AbortType=%d, AbortData=0x%x\n\r",
            slFatalErrorEvent->Data.DeviceAssert.Code,
            slFatalErrorEvent->Data.DeviceAssert.Value);
    }
    break;

    case SL_DEVICE_EVENT_FATAL_DRIVER_ABORT:
    {
        UART_PRINT("[ERROR] - FATAL ERROR: Driver Abort detected. \n\r");
    }
    break;

    case SL_DEVICE_EVENT_FATAL_NO_CMD_ACK:
    {
        UART_PRINT(
            "[ERROR] - FATAL ERROR: No Cmd Ack detected [cmd opcode = 0x%x] \n\r",
            slFatalErrorEvent->Data.NoCmdAck.Code);
    }
    break;

    case SL_DEVICE_EVENT_FATAL_SYNC_LOSS:
    {
        UART_PRINT("[ERROR] - FATAL ERROR: Sync loss detected n\r");
    }
    break;

    case SL_DEVICE_EVENT_FATAL_CMD_TIMEOUT:
    {
        UART_PRINT(
            "[ERROR] - FATAL ERROR: Async event timeout detected [event opcode =0x%x]  \n\r",
            slFatalErrorEvent->Data.CmdTimeout.Code);
    }
    break;

    default:
        UART_PRINT("[ERROR] - FATAL ERROR: Unspecified error detected \n\r");
        break;
    }
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
    switch(pNetAppEvent->Id)
    {
    case SL_NETAPP_EVENT_IPV4_ACQUIRED:
    {
        SET_STATUS_BIT(PowerMeasure_CB.slStatus, STATUS_BIT_IP_ACQUIRED);
        UART_PRINT("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
                   "Gateway=%d.%d.%d.%d\n\r",
                   SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,3),
                   SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,2),
                   SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,1),
                   SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,0),
                   SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,3),
                   SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,2),
                   SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,1),
                   SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,0));
    }
    break;

    default:
    {
        UART_PRINT("[NETAPP EVENT] Unexpected event [0x%x] \n\r",
                   pNetAppEvent->Id);
    }
    break;
    }
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
                                      SlNetAppHttpServerResponse_t *
                                      pHttpResponse)
{
    // Unused in this application
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
    //
    // Most of the general errors are not FATAL are are to be handled
    // appropriately by the application
    //
    UART_PRINT("[GENERAL EVENT] - ID=[%d] Sender=[%d]\n\n",
               pDevEvent->Data.Error.Code,
               pDevEvent->Data.Error.Source);
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
    //
    // This application doesn't work w/ socket - Events are not expected
    //
    switch(pSock->Event)
    {
    case SL_SOCKET_TX_FAILED_EVENT:
        switch(pSock->SocketAsyncEvent.SockTxFailData.Status)
        {
        case SL_ERROR_BSD_ECLOSE:
            UART_PRINT("[SOCK ERROR] - close socket (%d) operation "
                       "failed to transmit all queued packets\n\r",
                       pSock->SocketAsyncEvent.SockTxFailData.Sd);
            break;
        default:
            UART_PRINT(
                "[SOCK ERROR] - TX FAILED  :  socket %d , reason "
                "(%d) \n\n",
                pSock->SocketAsyncEvent.SockTxFailData.Sd,
                pSock->SocketAsyncEvent.SockTxFailData.Status);
            break;
        }
        break;

    default:
        UART_PRINT("[SOCK EVENT] - Unexpected Event [%x0x]\n\n",pSock->Event);
        break;
    }
}

void SimpleLinkSocketTriggerEventHandler(SlSockTriggerEvent_t *pSlTriggerEvent)
{
    // Unused in this application
}

//*****************************************************************************
// Local Functions
//*****************************************************************************

//*****************************************************************************
//
//! \brief Display Application Banner
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
int32_t displayBanner(void)
{
    int32_t status = -1;
    uint8_t macAddress[SL_MAC_ADDR_LEN];
    uint16_t macAddressLen = SL_MAC_ADDR_LEN;
    uint16_t configSize = 0;
    uint8_t configOpt = SL_DEVICE_GENERAL_VERSION;
    SlDeviceVersion_t ver = {0};

    configSize = sizeof(SlDeviceVersion_t);
    status = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(status);

    /* Print device version info. */
    status =
        sl_DeviceGet(SL_DEVICE_GENERAL, &configOpt, &configSize,
                     (uint8_t*)(&ver));
    ASSERT_ON_ERROR(status);

    /* Print device Mac address */
    status = sl_NetCfgGet(SL_NETCFG_MAC_ADDRESS_GET, 0, &macAddressLen,
                          &macAddress[0]);
    ASSERT_ON_ERROR(status);

    UART_PRINT("\n\n\n\r");
    UART_PRINT("\t ==============================================\n\r");
    UART_PRINT("\t    %s Example Ver: %s\n\r",APPLICATION_NAME,
               APPLICATION_VERSION);
    UART_PRINT("\t ==============================================\n\r");
    UART_PRINT("\n\r");
    UART_PRINT("\t CHIP: 0x%x",ver.ChipId);
    UART_PRINT("\n\r");
    UART_PRINT("\t MAC:  %d.%d.%d.%d",ver.FwVersion[0],ver.FwVersion[1],
               ver.FwVersion[2],
               ver.FwVersion[3]);
    UART_PRINT("\n\r");
    UART_PRINT("\t PHY:  %d.%d.%d.%d",ver.PhyVersion[0],ver.PhyVersion[1],
               ver.PhyVersion[2],
               ver.PhyVersion[3]);
    UART_PRINT("\n\r");
    UART_PRINT("\t NWP:  %d.%d.%d.%d",ver.NwpVersion[0],ver.NwpVersion[1],
               ver.NwpVersion[2],
               ver.NwpVersion[3]);
    UART_PRINT("\n\r");
    UART_PRINT("\t ROM:  %d",ver.RomVersion);
    UART_PRINT("\n\r");
    UART_PRINT("\t HOST: %s", SL_DRIVER_VERSION);
    UART_PRINT("\n\r");
    UART_PRINT("\t MAC address: %02x:%02x:%02x:%02x:%02x:%02x", macAddress[0],
               macAddress[1], macAddress[2], macAddress[3], macAddress[4],
               macAddress[5]);
    UART_PRINT("\n\r");
    UART_PRINT("\n\r");
    UART_PRINT("\t ==============================================\n\r");
    UART_PRINT("\n\r");
    UART_PRINT("\n\r");
    status = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(status);

    return(status);
}

//*****************************************************************************
//
//! \brief    Start measure nice printing.            .
//!
//! \param  None.
//!
//! \return None.
//
//*****************************************************************************
void startMeasureBanner(void)
{
    switch(PowerMeasure_appData.useCase)
    {
    case UseCase_HIB:
        UART_PRINT("Entering Hibernate, Start measure current..!\n\r");
        break;
    case UseCase_LPDS:
        UART_PRINT("Entering LPDS, Start measure current..!\n\r");
        break;
    case UseCase_Transceiver:
        UART_PRINT("Starting Transceiver mode, Start measure current..!\n\r");
        break;
    case UseCase_IntermittentlyConnected:
        UART_PRINT(
            "Starting Intermittentlly connected mode, Start measure current..!\n\r");
        break;
    case UseCase_AlwaysConnected:
        UART_PRINT(
            "Starting Always connected mode, Start measure current..!\n\r");
        break;
    }
    /* waiting for uart to flush out the buffer, 1ms timeout */
    ClockP_usleep(1000);
}

//****************************************************************************
//
//! \brief Connecting to a WLAN Access point
//!
//!  This function connects to the required AP (SSID_NAME) with Security
//!  parameters specified in te form of macros at the top of this file
//!
//! \param  None
//!
//! \return  None
//!
//! \warning    If the WLAN connection fails or we don't acquire an IP
//!            address, It will be stuck in this function forever.
//
//****************************************************************************
int32_t wlanConnect(void)
{
    SlWlanSecParams_t secParams = {0};
    int32_t status = 0;

    secParams.Key = (signed char*)SECURITY_KEY;
    secParams.KeyLen = strlen(SECURITY_KEY);
    secParams.Type = SECURITY_TYPE;

    status = sl_WlanConnect((signed char*)SSID_NAME, strlen(
                                SSID_NAME), 0, &secParams, 0);
    ASSERT_ON_ERROR(status);

    UART_PRINT("Trying to connect to AP : %s\n\r", SSID_NAME);
    sl_Task(NULL);
    // Wait for WLAN Event
    while((!IS_CONNECTED(PowerMeasure_CB.slStatus)) ||
          (!IS_IP_ACQUIRED(PowerMeasure_CB.slStatus)))
    {
        /* Turn on user LED */
        GPIO_write(Board_LED0, Board_LED_ON);
        ClockP_usleep(50000);
        /* Turn off user LED */
        GPIO_write(Board_LED0, Board_LED_OFF);
        ClockP_usleep(50000);
        sl_Task(NULL);
    }

    return(0);
}

//*****************************************************************************
//
//! Check the device mode and switch to STATION(STA) mode
//! restart the NWP to activate STATION mode
//!
//! \param  iMode (device mode)
//!
//! \return None
//
//*****************************************************************************
void switchToStaMode(int32_t mode)
{
    int32_t status = -1;

    if(mode != ROLE_STA)
    {
        status = sl_WlanSetMode(ROLE_STA);
        if(status < 0)
        {
            ERR_PRINT(status);
            LOOP_FOREVER();
        }
        sl_Stop(0xFF);
        if(status < 0)
        {
            ERR_PRINT(status);
            LOOP_FOREVER();
        }
        //
        // Assumption is that the device is configured in station mode already
        // and it is in its default state
        //
        status = sl_Start(0, 0, 0);
        if(status < 0 || ROLE_STA != status)
        {
            UART_PRINT("Failed to start the device \n\r");
            LOOP_FOREVER();
        }
    }
    CLR_STATUS_BIT(PowerMeasure_CB.slStatus, STATUS_BIT_CONNECTION);
    CLR_STATUS_BIT(PowerMeasure_CB.slStatus, STATUS_BIT_IP_ACQUIRED);
}

//*****************************************************************************
//
//! \brief    Get from the user the selected Power Management use case.
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void setUseCase(void)
{
    char sel[2];

    UART_PRINT("*** Power management use case Options : *** \n\r");
    UART_PRINT("    1) for Hibernate.  \n\r");
    UART_PRINT("    2) for LPDS.  \n\r");
    UART_PRINT("    3) for Transceiver Mode  \n\r");
    UART_PRINT("    4) for Intermittently Connected.  \n\r");
    UART_PRINT("    5) for Always Connected  \n\r");
    UART_PRINT("Please enter your Power management use case selection:  ");
    GetCmd(sel,sizeof(sel));
    UART_PRINT("\n\r");
    PowerMeasure_appData.useCase = (UseCases)(atoi((const char*)sel) - 1);
    return;
}

//*****************************************************************************
//
//! \brief    Configure the device according to selected Power Management use case.
//!
//! \param  None
//!
//! \return Success or Fail
//
//*****************************************************************************
int32_t configSimplelinkToUseCase(void)
{
    int32_t status = -1;
    SlWlanPmPolicyParams_t PmPolicyParams;

    memset(&PmPolicyParams,0,sizeof(SlWlanPmPolicyParams_t));
    PmPolicyParams.MaxSleepTimeMs = LSI_DURATION_IN_MSEC;

    switch(PowerMeasure_appData.useCase)
    {
    case UseCase_Transceiver:
        /* No connection */
        status =
            sl_WlanPolicySet(SL_WLAN_POLICY_CONNECTION,
                             SL_WLAN_CONNECTION_POLICY(0, 0, 0,
                                                       0), NULL, 0);
        ASSERT_ON_ERROR(status);
        /* Low Power Policy */
        status =
            sl_WlanPolicySet(SL_WLAN_POLICY_PM, SL_WLAN_LOW_POWER_POLICY, NULL,
                             0);
        ASSERT_ON_ERROR(status);
        setHibRetentionReg(UseCase_Transceiver);
        break;
    case UseCase_IntermittentlyConnected:
        /* Connection policy Auto + fast */
        status =
            sl_WlanPolicySet(SL_WLAN_POLICY_CONNECTION,
                             SL_WLAN_CONNECTION_POLICY(1, 1, 0,
                                                       0), NULL, 0);
        ASSERT_ON_ERROR(status);
        setIpAddrAllocMode(IP_ADDR_ALLOC_MODE);
        break;
    case UseCase_AlwaysConnected:
        /* Connection policy Auto  */
        status =
            sl_WlanPolicySet(SL_WLAN_POLICY_CONNECTION,
                             SL_WLAN_CONNECTION_POLICY(1, 0, 0,
                                                       0), NULL, 0);
        ASSERT_ON_ERROR(status);
        /* LSI setting */
        if(LSI_DURATION_IN_MSEC > 100)
        {
            sl_WlanPolicySet(SL_WLAN_POLICY_PM,
                             SL_WLAN_LONG_SLEEP_INTERVAL_POLICY,
                             (uint8_t *)&PmPolicyParams,
                             sizeof(PmPolicyParams));
        }
        setIpAddrAllocMode(IP_ADDR_ALLOC_MODE);
        break;
    default:
        /* No connection */
        status =
            sl_WlanPolicySet(SL_WLAN_POLICY_CONNECTION,
                             SL_WLAN_CONNECTION_POLICY(0, 0, 0,
                                                       0), NULL, 0);
        ASSERT_ON_ERROR(status);
        break;
    }
    status = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(status);

    return(status);
}

//*****************************************************************************
//
//! \brief    Set the IP address alloccation method for the connected use cases.
//!
//! \param  Mode (the allocation method)
//!
//! \return Success or Fail
//
//*****************************************************************************
int32_t setIpAddrAllocMode(uint8_t mode)
{
    int32_t status = -1;
    SlNetCfgIpV4Args_t ipV4;

    switch(mode)
    {
    case STATIC_IP:
        ipV4.Ip = (uint32_t)SRC_IP_ADDR;
        ipV4.IpMask = (uint32_t)SL_IPV4_VAL(255,255,255,0);
        ipV4.IpGateway = (uint32_t)GATEWAY_IP_ADDR;
        ipV4.IpDnsServer = (uint32_t)GATEWAY_IP_ADDR;
        status =
            sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE,SL_NETCFG_ADDR_STATIC,
                         sizeof(SlNetCfgIpV4Args_t),
                         (uint8_t *)&ipV4);
        break;
    case DHCP_NO_FAST_RENEW:
        status =
            sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE, SL_NETCFG_ADDR_DHCP, 0,
                         0);
        /* Disable the "fast renew feature" */
        status =
            sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE,
                         SL_NETCFG_ADDR_DISABLE_FAST_RENEW,
                         0,
                         0);
        break;
    case DHCP_FAST_RENEW_NO_ACK:
        status =
            sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE, SL_NETCFG_ADDR_DHCP, 0,
                         0);
        /* Enable the "fast renew feature" + "NO wait" */
        status =
            sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE,
                         SL_NETCFG_ADDR_ENABLE_FAST_RENEW,
                         0,
                         0);
        status =
            sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE,
                         SL_NETCFG_ADDR_FAST_RENEW_MODE_NO_WAIT_ACK,
                         0,
                         0);
        break;
    case DHCP_FAST_RENEW_WAIT_ACK:
        status =
            sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE, SL_NETCFG_ADDR_DHCP, 0,
                         0);
        /* Enable the "fast renew feature" + "wait ack" */
        status =
            sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE,
                         SL_NETCFG_ADDR_ENABLE_FAST_RENEW,
                         0,
                         0);
        status =
            sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE,
                         SL_NETCFG_ADDR_FAST_RENEW_MODE_WAIT_ACK,
                         0,
                         0);
        break;
    }
    return(status);
}

//*****************************************************************************
//
//! \brief This function practice Transceiver mode.
//!
//! \param None.
//!
//! \return 0 on success, -ve otherwise.
//
//*****************************************************************************
int32_t transceiverMode(void)
{
    int32_t status = -1;
    int32_t socketId;
    int32_t idx;

    prepareDataFrame(PowerMeasure_appData.port,PowerMeasure_appData.ipAddr);

    status = sl_Start(0,0,0);
    ASSERT_ON_ERROR(status);
    if(CCA_BYPASS)
    {
        /* bypass the cca */
        socketId = sl_Socket(SL_AF_RF,SL_SOCK_RAW,0);
    }
    else
    {
        /* consider cca */
        socketId = sl_Socket(SL_AF_RF,SL_SOCK_DGRAM,0);
    }

    for(idx = 0; idx < NUM_OF_PKT; idx++)
    {
        status =
            sl_Send(socketId,PowerMeasure_CB.frameData,FRAME_LENGTH,
                    SL_WLAN_RAW_RF_TX_PARAMS(TAG_CHANNEL,
                                             TAG_FRAME_TRANSMIT_RATE,
                                             TAG_FRAME_TRANSMIT_POWER,
                                             1));
        ClockP_usleep(5000);
    }
    status = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(status);

    /* enter hibernate */
    powerShutdown(NOT_ACTIVE_DURATION_MSEC);

    /* this line is never Reached */
    return(status);
}

//*****************************************************************************
//
//! \brief This function practice Always connected mode.
//!
//! \param None.
//!
//! \return 0 on success, -ve otherwise.
//
//*****************************************************************************
int32_t alwaysConnected(void)
{
    int32_t status = -1;

    switch(PowerMeasure_appData.socketType)
    {
    case SocketType_UDP:
        status = bsdUdpClient(PowerMeasure_appData.port,
                              PowerMeasure_appData.sockID);
        ASSERT_ON_ERROR(status);
        break;
    case SocketType_TCP:
        status = bsdTcpClient(PowerMeasure_appData.port,
                              PowerMeasure_appData.sockID);
        ASSERT_ON_ERROR(status);
        break;
    case SocketType_SEC_TCP:
        status = bsdTcpSecClient(PowerMeasure_appData.port,
                                 PowerMeasure_appData.sockID);
        ASSERT_ON_ERROR(status);
        break;
    }
    return(status);
}

//*****************************************************************************
//
//! \brief This function practice Intermittently connected mode.
//!
//! \param None.
//!
//! \return 0 on success, -ve otherwise.
//
//*****************************************************************************
int32_t intermittentlyConnected(void)
{
    int32_t status = -1;

    prepareDataFrame(PowerMeasure_appData.port,PowerMeasure_appData.ipAddr);

    status = sl_Start(0,0,0);
    ASSERT_ON_ERROR(status);

    /* waiting for fast connect proccess to compleate */
    sl_Task(NULL);
    while(!IS_IP_ACQUIRED(PowerMeasure_CB.slStatus))
    {
        sl_Task(NULL);
        ;
    }

    switch(PowerMeasure_appData.socketType)
    {
    case SocketType_UDP:
        status = bsdUdpClient(PowerMeasure_appData.port,ALWAYS_OPEN_SOCK);
        ASSERT_ON_ERROR(status);
        break;
    case SocketType_TCP:
        status = bsdTcpClient(PowerMeasure_appData.port,ALWAYS_OPEN_SOCK);
        ASSERT_ON_ERROR(status);
        break;
    case SocketType_SEC_TCP:
        status = bsdTcpSecClient(PowerMeasure_appData.port,ALWAYS_OPEN_SOCK);
        ASSERT_ON_ERROR(status);
        break;
    }

    status = sl_Stop(SL_STOP_TIMEOUT);
    /* enter hibernate */
    powerShutdown(NOT_ACTIVE_DURATION_MSEC);
    /* this line is never Reached */
    /* clear SimpleLink Status  for the MSP432 Plugin, because we don't got to HIB */
    PowerMeasure_CB.slStatus = 0;
    return(status);
}

//*****************************************************************************
//
//! \brief This function implement UDP client .
//!
//! \param Port - socket port number; Sid - socket id, -ve if socket is alrady opened.
//!
//! \return 0 on success, -ve otherwise.
//
//*****************************************************************************
int32_t bsdUdpClient(uint16_t port,
                     int16_t sid)
{
    int16_t sockId;
    int16_t idx = 0;
    int32_t status = -1;

    if(sid < 0)
    {
        /* Need to open socket  */
        sockId = sl_Socket(SL_AF_INET,SL_SOCK_DGRAM, 0);
        ASSERT_ON_ERROR(sockId);
    }
    else
    {
        /* Socket is already opened */
        sockId = sid;
    }

    while(idx < NUM_OF_PKT)
    {
        status = sl_SendTo(sockId,PowerMeasure_CB.frameData,FRAME_LENGTH, 0,
                           (SlSockAddr_t *)&PowerMeasure_CB.ipV4Addr,
                           sizeof(SlSockAddrIn_t));
        if(status <= 0)
        {
            status = sl_Close(sockId);
            ASSERT_ON_ERROR(sockId);
        }
        idx++;
    }

    if(sid == ALWAYS_OPEN_SOCK)
    {
        /* Next time, use a new socket */
        status = sl_Close(sockId);
        ASSERT_ON_ERROR(status);
    }
    else
    {
        /* store the current open socket id*/
        PowerMeasure_appData.sockID = sockId;
    }

    return(0);
}

//*****************************************************************************
//
//! \brief This function implement TCP client .
//!
//! \param Port - socket port number; Sid - socket id, -ve if socket is alrady opened.
//!
//! \return 0 on success, -ve otherwise.
//
//*****************************************************************************
int32_t bsdTcpClient(uint16_t port,
                     int16_t sid)
{
    int16_t sockId;
    int16_t idx = 0;
    int16_t status = -1;

    if(sid < 0)
    {
        /* Need to open socket  */
        sockId = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
        ASSERT_ON_ERROR(sockId);
        /* Make connection establishment */
        status = sl_Connect(sockId, ( SlSockAddr_t *)&PowerMeasure_CB.ipV4Addr,
                            sizeof(SlSockAddrIn_t));
        if(status < 0)
        {
            sl_Close(sockId);
            ASSERT_ON_ERROR(sockId);
        }
    }
    else
    {
        /* Socket is already opened */
        sockId = sid;
    }

    while(idx < NUM_OF_PKT)
    {
        status = sl_Send(sockId,PowerMeasure_CB.frameData,FRAME_LENGTH, 0);
        if(status <= 0)
        {
            status = sl_Close(sockId);
            ASSERT_ON_ERROR(status);
        }
        idx++;
    }
    if(sid == ALWAYS_OPEN_SOCK)
    {
        /* Next time, use a new socket */
        status = sl_Close(sockId);
        ASSERT_ON_ERROR(status);
    }
    else
    {
        /* store the current open socket id*/
        PowerMeasure_appData.sockID = sockId;
    }
    return(0);
}

//*****************************************************************************
//
//! \brief This function implement TLS/SLL on TCP client .
//!
//! \param Port - socket port number; Sid - socket id, -ve if socket is alrady opened.
//!
//! \return 0 on success, -ve otherwise.
//
//*****************************************************************************
int32_t bsdTcpSecClient(uint16_t port,
                        int16_t sid)
{
    uint32_t cipher = SL_SEC_MASK_SSL_RSA_WITH_RC4_128_SHA;
    uint8_t method = SL_SO_SEC_METHOD_SSLV3;
    int16_t sockId;
    int16_t idx = 0;
    int32_t status = -1;

    if(sid < 0)
    {
        /* need to open new socket */
        sockId = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, SL_SEC_SOCKET);
        ASSERT_ON_ERROR(sockId);
        /* Make connection establishment */
        status = sl_SetSockOpt(sockId, SL_SOL_SOCKET, SL_SO_SECMETHOD,
                               &method, sizeof(method));
        if(status < 0)
        {
            UART_PRINT(" Failed to configure the socket \n\r");
            ASSERT_ON_ERROR(status);
        }
        status = sl_SetSockOpt(sockId, SL_SOL_SOCKET, SL_SO_SECURE_MASK,
                               &cipher, sizeof(cipher));
        if(status < 0)
        {
            UART_PRINT(" Failed to configure the socket \n\r");
            ASSERT_ON_ERROR(status);
        }
        /* If the user flashed the server certificate the lines below can be uncomment  */
        /*
           status = sl_SetSockOpt(g_SockID, SL_SOL_SOCKET, SL_SO_SECURE_FILES_CA_FILE_NAME,
                SL_SSL_CA_CERT, pal_Strlen(SL_SSL_CA_CERT));
           if( status < 0 )
           {
            UART_PRINT(" Failed to configure the socket \n\r");
            LOOP_FOREVER();
           }
         */
        /* connect to the peer server */
        status = sl_Connect(sockId, ( SlSockAddr_t *)&PowerMeasure_CB.ipV4Addr,
                            sizeof(SlSockAddrIn_t));
        if((status < 0) && (status != SL_ERROR_BSD_ESECSNOVERIFY))
        {
            /* ignore authentication error */
            UART_PRINT(" Failed to connect w/ server \n\r");
            LOOP_FOREVER();
        }
        UART_PRINT(" Connection w/ server established successfully \n\r");
    }
    else
    {
        sockId = sid;
    }

    while(idx < NUM_OF_PKT)
    {
        status = sl_Send(sockId, PowerMeasure_CB.frameData, FRAME_LENGTH, 0);
        if(status <= 0)
        {
            UART_PRINT(" [TCP Client] Data send Error \n\r");
            status = sl_Close(sockId);
            ASSERT_ON_ERROR(status);
        }
        idx++;
    }

    if(sid == ALWAYS_OPEN_SOCK)
    {
        /* Next time, use a new socket */
        status = sl_Close(sockId);
        ASSERT_ON_ERROR(status);
    }
    else
    {
        /* store the current open socket id */
        PowerMeasure_appData.sockID = sockId;
    }
    return(0);
}

//*****************************************************************************
//
//! \brief    This function prepare the Data Frame & initialize IP address data
//!         struct            .
//!
//! \param  Port & IP addressnn
//!
//! \return None
//
//*****************************************************************************
void prepareDataFrame(uint16_t port,
                      uint32_t ipAddr)
{
    uint16_t idx;

    for(idx = 0; idx < FRAME_LENGTH; idx++)
    {
        PowerMeasure_CB.frameData[idx] = (signed char)(idx % 255);
    }
    PowerMeasure_CB.ipV4Addr.sin_family = SL_AF_INET;
    PowerMeasure_CB.ipV4Addr.sin_port = sl_Htons(port);
    PowerMeasure_CB.ipV4Addr.sin_addr.s_addr = sl_Htonl(ipAddr);
}

//*****************************************************************************
//
//! \brief  Do nothing
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void powerMeasureDoNothing()
{
    //Do nothing ...
}

//*****************************************************************************
//
//! \brief Task Created by main function.This task starts simpleLink, set NWP
//!        power policy, connects to an AP. Give Signal to the other task about
//!        the connection.wait for the message form the interrupt handlers and
//!        the other task. Accordingly print the wake up cause from the low
//!        power modes.
//!
//! \param pvParameters is a general void pointer (not used here).
//!
//! \return none
//
//*****************************************************************************
void mainThread(void *pvParameters)
{
    int32_t status = 0;
    int16_t slStarted = 0;
    ClockP_Params lpdsParams;

    Board_initGPIO();
    Board_initSPI();

    /* Configure the UART */
    InitTerm();

    /* Turn on user LED */
    GPIO_write(Board_LED0, Board_LED_ON);

    /* clear SimpleLink Status */
    PowerMeasure_CB.slStatus = 0;

    ClockP_Params_init(&lpdsParams);
    PowerMeasure_lpdsHandle = ClockP_create(
        (ClockP_Fxn) & powerMeasureDoNothing, 0, &lpdsParams);

    if(isWokenFromHib())
    {
        UART_PRINT("Woken from Hib.\n\r");
        /* When waking from Hibernate we need to determine what use case the app exercises */
        if(getHibRetentionReg() == (uint8_t) UseCase_Transceiver)
        {
            PowerMeasure_appData.useCase = UseCase_Transceiver;
        }
        else
        {
            PowerMeasure_appData.useCase = UseCase_IntermittentlyConnected;
        }
    }
    else
    {
        /* Before turning on the NWP on,
           reset any previously configured parameters */
        /*
           IMPORTANT NOTE - This is an example reset function,
           user must update this function to match the application settings.
         */
        status = sl_WifiConfig();
        if(status < 0)
        {
            /* Handle Error */
            UART_PRINT(
                "Power Measurement - Couldn't configure Network Processor - %d\n",
                status);
            LOOP_FOREVER();
        }

        /* Displays the Application Banner */
        displayBanner();

        /* Turn NWP on */
        status = sl_Start(NULL, NULL, NULL);
        if(status < 0)
        {
            /* Handle Error */
            UART_PRINT("sl_start failed - %d\n",status);
            LOOP_FOREVER();
        }

        /* Unregister mDNS services */
        status = sl_NetAppMDNSUnRegisterService(0, 0, 0);
        if(status < 0)
        {
            /* Handle Error */
            UART_PRINT("sl_NetAppMDNSUnRegisterService failed - %d\n",status);
            LOOP_FOREVER();
        }

        /* Set Desired PM use case */
        setUseCase();

        /* configure device to the selected use-case */
        status = configSimplelinkToUseCase();
        UART_PRINT(
            "Device Configured to the selected Power Management use case.\n\r");

        /* Connect To AP for first time, connected use cases only */
        if(PowerMeasure_appData.useCase == UseCase_AlwaysConnected ||
           PowerMeasure_appData.useCase == UseCase_IntermittentlyConnected)
        {
            status = sl_Start(0,0,0);
            status = wlanConnect();
            /* Stay connected if the use case is always connected */
            if(PowerMeasure_appData.useCase == UseCase_IntermittentlyConnected)
            {
                status = sl_Stop(SL_STOP_TIMEOUT);
            }
        }
        startMeasureBanner();
    }

    while(1)
    {
        /* The SimpleLink host driver architecture mandate calling 'sl_task' in a NO-RTOS application's main loop.       */
        /* The purpose of this call, is to handle asynchronous events and get flow control information sent from the NWP.*/
        /* Every event is classified and later handled by the host driver event handlers.                                */
        sl_Task(NULL);

        switch(PowerMeasure_appData.useCase)
        {
        case UseCase_HIB:
            powerShutdown(MAX_INT);
            break;
        case UseCase_LPDS:
            if(slStarted == 0)
            {
                status = sl_Start(0,0,0);
                if(status < 0)
                {
                    UART_PRINT("unable to start the device \n\r");
                    LOOP_FOREVER();
                }
                slStarted = 1;
            }
            /* Enable LPDS policy */
            Power_enablePolicy();
            Power_idleFunc();
            break;
        case UseCase_Transceiver:
            UART_PRINT("Transceiving...\n\r");
            transceiverMode();
            break;
        case UseCase_IntermittentlyConnected:
            UART_PRINT("Intermittently Connecting...\n\r");
            intermittentlyConnected();
            break;
        case UseCase_AlwaysConnected:
            prepareDataFrame(PowerMeasure_appData.port,
                             PowerMeasure_appData.ipAddr);
            /* Enable LPDS policy */
            Power_enablePolicy();
            ClockP_setTimeout(PowerMeasure_lpdsHandle,LPDS_IDLE_TIME);
            ClockP_start(PowerMeasure_lpdsHandle);
            Power_idleFunc();
            alwaysConnected();
            break;
        default:
            break;
        }
    }
}
