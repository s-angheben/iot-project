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

   Application Name     -   MQTT Client
   Application Overview -   The device is running a MQTT client which is
                           connected to the online broker. Three LEDs on the
                           device can be controlled from a web client by
                           publishing msg on appropriate topics. Similarly,
                           message can be published on pre-configured topics
                           by pressing the switch buttons on the device.

   Application Details  - Refer to 'MQTT Client' README.html

*****************************************************************************/
#include <mqtt_if.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <mqueue.h>

#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/net/wifi/slnetifwifi.h>

#include <ti/drivers/SPI.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Timer.h>

#include <ti/net/mqtt/mqttclient.h>

#include "network_if.h"
#include "uart_term.h"
#include "mqtt_if.h"
#include "debug_if.h"

#include "ti_drivers_config.h"

#define APPLICATION_NAME         "MQTT client"
#define APPLICATION_VERSION      "2.0.0"

#define SL_TASKSTACKSIZE            2048
#define SPAWN_TASK_PRIORITY         9

#define SLNET_IF_WIFI_PRIO          (5)

// un-comment this if you want to connect to an MQTT broker securely
//#define MQTT_SECURE_CLIENT

#define MQTT_MODULE_TASK_PRIORITY   2
#define MQTT_MODULE_TASK_STACK_SIZE 2048

#define MQTT_WILL_TOPIC             "cc32xx_will_topic"
#define MQTT_WILL_MSG               "will_msg_works"
#define MQTT_WILL_QOS               MQTT_QOS_2
#define MQTT_WILL_RETAIN            false

#define MQTT_CLIENT_PASSWORD        NULL
#define MQTT_CLIENT_USERNAME        NULL
#define MQTT_CLIENT_KEEPALIVE       0
#define MQTT_CLIENT_CLEAN_CONNECT   true
#define MQTT_CLIENT_MQTT_V3_1       true
#define MQTT_CLIENT_BLOCKING_SEND   true

#define MQTT_CONNECTION_FLAGS           MQTTCLIENT_NETCONN_URL
#define MQTT_CONNECTION_ADDRESS         "192.168.1.6"
#define MQTT_CONNECTION_PORT_NUMBER     1883


mqd_t appQueue;
int connected;
int deinit;
Timer_Handle timer0;
int longPress = 0;

// dht11 stuff
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#define HIGH 0
#define LOW 0
UInt32 last = 0;
UInt32 now = 0;
UInt32 savedate[45];
int counter = 0;
Timer_Handle timer1;
Timer_Params params1;

void countHandler(uint_least8_t index)
{
//    now = Clock_getTicks();
    now = Timer_getCount(timer1);
    savedate[counter] = (now - last);
    last = now;
    counter++;
}
void read_dht11(){

    // pin low for 18ms
    GPIO_write(dht11, LOW);
    Task_sleep(18000 / Clock_tickPeriod);

    // pin high
    GPIO_write(dht11, HIGH);

    GPIO_setConfig(dht11, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    GPIO_setCallback(dht11, countHandler);
    /* Enable interrupts */
//    last = Clock_getTicks();



    Timer_Params_init(&params1);
    params1.period = 5000000;
    params1.periodUnits = Timer_PERIOD_US;
    params1.timerMode = Timer_FREE_RUNNING;
    params1.timerCallback = NULL;

    timer1 = Timer_open(Board_TIMER1, &params1);

    if (timer1 == NULL) {
            /* Failed to initialized timer */
        while (1) {}
    }

    if (Timer_start(timer1) == Timer_STATUS_ERROR) {
        /* Failed to start timer */
        while (1) {}
    }

    last = Timer_getCount(timer1);
    GPIO_enableInt(dht11);

    // wait
    Task_sleep(25000 / Clock_tickPeriod);

    int j,i, c=1;
    uint8_t bytes[5];
    for (i = 0; i < 5; i++) bytes[i] = 0;
    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 8; j++)
        {
            /* shift in the data, msb first if width > threshold */
            bytes[i] |= ((savedate[c] > 4000) << (7 - j));
            c++;
        }
    }

    /* the checksum will overflow automatically */
    uint8_t checkSum = 0;
    for (i = 0; i < (5 - 1); i++)  checkSum += bytes[i];
    if (checkSum != bytes[4]) while(1);

    int humidity    = bytes[0];
    int temperature = bytes[2];
    LOG_INFO("temperatura: %d, umidita: %d \n", temperature, humidity);
}


/* Client ID                                                                 */
/* If ClientId isn't set, the MAC address of the device will be copied into  */
/* the ClientID parameter.                                                   */
char ClientId[13] = {"board0"};

enum
{
    APP_MQTT_PUBLISH,
    APP_MQTT_CON_TOGGLE,
    APP_MQTT_DEINIT,
    APP_BTN_HANDLER
};

struct msgQueue
{
    int event;
    char* payload;
};

MQTT_IF_InitParams_t mqttInitParams =
{
    MQTT_MODULE_TASK_STACK_SIZE,    // stack size for mqtt module - default is 2048
    MQTT_MODULE_TASK_PRIORITY       // thread priority for MQTT   - default is 2
};

MQTTClient_Will mqttWillParams =
{
    MQTT_WILL_TOPIC,     // will topic
    MQTT_WILL_MSG,       // will message
    MQTT_WILL_QOS,       // will QoS
    MQTT_WILL_RETAIN     // retain flag
};

MQTT_IF_ClientParams_t mqttClientParams =
{
    ClientId,                   // client ID
    MQTT_CLIENT_USERNAME,       // user name
    MQTT_CLIENT_PASSWORD,       // password
    MQTT_CLIENT_KEEPALIVE,      // keep-alive time
    MQTT_CLIENT_CLEAN_CONNECT,  // clean connect flag
    MQTT_CLIENT_MQTT_V3_1,      // true = 3.1, false = 3.1.1
    MQTT_CLIENT_BLOCKING_SEND,  // blocking send flag
    &mqttWillParams             // will parameters
};

MQTTClient_ConnParams mqttConnParams =
{
    MQTT_CONNECTION_FLAGS,          // connection flags
    MQTT_CONNECTION_ADDRESS,        // server address
    MQTT_CONNECTION_PORT_NUMBER,    // port number of MQTT server
    0,                              // method for secure socket
    0,                              // cipher for secure socket
    0,                              // number of files for secure connection
    NULL                            // secure files
};


void timerCallback(Timer_Handle myHandle)
{
    longPress = 1;
}

// this timer callback toggles the LED once per second until the device connects to an AP
void timerLEDCallback(Timer_Handle myHandle)
{
    GPIO_toggle(Board_GPIO_LED0);
}

void pushButtonPublishHandler(uint_least8_t index)
{
    int ret;
    struct msgQueue queueElement;

    GPIO_disableInt(Board_BUTTON0);

    queueElement.event = APP_MQTT_PUBLISH;
    ret = mq_send(appQueue, (const char*)&queueElement, sizeof(struct msgQueue),
                  0);
    if(ret < 0)
    {
        LOG_ERROR("msg queue send error %d", ret);
    }
}

void pushButtonConnectionHandler(uint_least8_t index)
{
    int ret;
    struct msgQueue queueElement;

    GPIO_disableInt(Board_BUTTON1);

    ret = Timer_start(timer0);
    if(ret < 0)
    {
        LOG_ERROR("failed to start the timer\r\n");
    }

    queueElement.event = APP_BTN_HANDLER;

    ret = mq_send(appQueue, (const char*)&queueElement, sizeof(struct msgQueue),
                  0);
    if(ret < 0)
    {
        LOG_ERROR("msg queue send error %d", ret);
    }
}

int detectLongPress(){
    int buttonPressed;

    do
    {
        buttonPressed = GPIO_read(Board_BUTTON1);
    }
    while(buttonPressed && !longPress);

    // disabling the timer in case the callback has not yet triggered to avoid updating longPress
    Timer_stop(timer0);

    if(longPress == 1)
    {
        longPress = 0;
        return(1);
    }
    else
    {
        return(0);
    }
}

void MQTT_EventCallback(int32_t event){
    struct msgQueue queueElement;

    switch(event)
    {
    case MQTT_EVENT_CONNACK:
    {
        deinit = 0;
        connected = 1;
        LOG_INFO("MQTT_EVENT_CONNACK\r\n");
        GPIO_clearInt(Board_BUTTON1);
        GPIO_enableInt(Board_BUTTON1);
        break;
    }

    case MQTT_EVENT_SUBACK:
    {
        LOG_INFO("MQTT_EVENT_SUBACK\r\n");
        break;
    }

    case MQTT_EVENT_PUBACK:
    {
        LOG_INFO("MQTT_EVENT_PUBACK\r\n");
        break;
    }

    case MQTT_EVENT_UNSUBACK:
    {
        LOG_INFO("MQTT_EVENT_UNSUBACK\r\n");
        break;
    }

    case MQTT_EVENT_CLIENT_DISCONNECT:
    {
        connected = 0;
        LOG_INFO("MQTT_EVENT_CLIENT_DISCONNECT\r\n");
        if(deinit == 0)
        {
            GPIO_clearInt(Board_BUTTON1);
            GPIO_enableInt(Board_BUTTON1);
        }
        break;
    }

    case MQTT_EVENT_SERVER_DISCONNECT:
    {
        connected = 0;

        LOG_INFO("MQTT_EVENT_SERVER_DISCONNECT\r\n");

        queueElement.event = APP_MQTT_CON_TOGGLE;
        int res =
            mq_send(appQueue, (const char*)&queueElement,
                    sizeof(struct msgQueue), 0);
        if(res < 0)
        {
            LOG_ERROR("msg queue send error %d", res);
        }
        break;
    }

    case MQTT_EVENT_DESTROY:
    {
        LOG_INFO("MQTT_EVENT_DESTROY\r\n");
        break;
    }
    }
}

/*
 * Subscribe topic callbacks
 * Topic and payload data is deleted after topic callbacks return.
 * User must copy the topic or payload data if it needs to be saved.
 */
void BrokerCB(char* topic,
              char* payload){
    LOG_INFO("TOPIC: %s \tPAYLOAD: %s\r\n", topic, payload);
}

void ToggleLED1CB(char* topic,
                  char* payload){
    GPIO_toggle(Board_GPIO_LED0);
    LOG_INFO("TOPIC: %s \tPAYLOAD: %s\r\n", topic, payload);
}

void ToggleLED2CB(char* topic,
                  char* payload){
    GPIO_toggle(Board_GPIO_LED1);
    LOG_INFO("TOPIC: %s \tPAYLOAD: %s\r\n", topic, payload);
}

void ToggleLED3CB(char* topic,
                  char* payload){
    GPIO_toggle(Board_GPIO_LED2);
    LOG_INFO("TOPIC: %s \tPAYLOAD: %s\r\n", topic, payload);
}

int32_t DisplayAppBanner(char* appName,
                         char* appVersion){
    int32_t ret = 0;
    uint8_t macAddress[SL_MAC_ADDR_LEN];
    uint16_t macAddressLen = SL_MAC_ADDR_LEN;
    uint16_t ConfigSize = 0;
    uint8_t ConfigOpt = SL_DEVICE_GENERAL_VERSION;
    SlDeviceVersion_t ver = {0};

    ConfigSize = sizeof(SlDeviceVersion_t);

    // get the device version info and MAC address
    ret =
        sl_DeviceGet(SL_DEVICE_GENERAL, &ConfigOpt, &ConfigSize,
                     (uint8_t*)(&ver));
    ret |= (int32_t)sl_NetCfgGet(SL_NETCFG_MAC_ADDRESS_GET, 0, &macAddressLen,
                                 &macAddress[0]);

    UART_PRINT("\n\r\t============================================\n\r");
    UART_PRINT("\t   %s Example Ver: %s",appName, appVersion);
    UART_PRINT("\n\r\t============================================\n\r\n\r");

    UART_PRINT("\t CHIP: 0x%x\n\r",ver.ChipId);
    UART_PRINT("\t MAC:  %d.%d.%d.%d\n\r",ver.FwVersion[0],ver.FwVersion[1],
               ver.FwVersion[2],
               ver.FwVersion[3]);
    UART_PRINT("\t PHY:  %d.%d.%d.%d\n\r",ver.PhyVersion[0],ver.PhyVersion[1],
               ver.PhyVersion[2],
               ver.PhyVersion[3]);
    UART_PRINT("\t NWP:  %d.%d.%d.%d\n\r",ver.NwpVersion[0],ver.NwpVersion[1],
               ver.NwpVersion[2],
               ver.NwpVersion[3]);
    UART_PRINT("\t ROM:  %d\n\r",ver.RomVersion);
    UART_PRINT("\t HOST: %s\n\r", SL_DRIVER_VERSION);
    UART_PRINT("\t MAC address: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
               macAddress[0],
               macAddress[1], macAddress[2], macAddress[3], macAddress[4],
               macAddress[5]);
    UART_PRINT("\n\r\t============================================\n\r");

    return(ret);
}

int WifiInit(){
    int32_t ret;
    SlWlanSecParams_t security_params;
    pthread_t spawn_thread = (pthread_t) NULL;
    pthread_attr_t pattrs_spawn;
    struct sched_param pri_param;

    pthread_attr_init(&pattrs_spawn);
    pri_param.sched_priority = SPAWN_TASK_PRIORITY;
    ret = pthread_attr_setschedparam(&pattrs_spawn, &pri_param);
    ret |= pthread_attr_setstacksize(&pattrs_spawn, SL_TASKSTACKSIZE);
    ret |= pthread_attr_setdetachstate(&pattrs_spawn, PTHREAD_CREATE_DETACHED);
    ret = pthread_create(&spawn_thread, &pattrs_spawn, sl_Task, NULL);
    if(ret != 0)
    {
        LOG_ERROR("could not create simplelink task\n\r");
        while(1)
        {
            ;
        }
    }

    Network_IF_ResetMCUStateMachine();

    Network_IF_DeInitDriver();

    ret = Network_IF_InitDriver(ROLE_STA);
    if(ret < 0)
    {
        LOG_ERROR("Failed to start SimpleLink Device\n\r");
        while(1)
        {
            ;
        }
    }

    DisplayAppBanner(APPLICATION_NAME, APPLICATION_VERSION);

    GPIO_toggle(Board_GPIO_LED2);

    security_params.Key = (signed char*)SECURITY_KEY;
    security_params.KeyLen = strlen(SECURITY_KEY);
    security_params.Type = SECURITY_TYPE;

    ret = Timer_start(timer0);
    if(ret < 0)
    {
        LOG_ERROR("failed to start the timer\r\n");
    }

    ret = Network_IF_ConnectAP(SSID_NAME, security_params);
    if(ret < 0)
    {
        LOG_ERROR("Connection to an AP failed\n\r");
    }
    else
    {
        SlWlanSecParams_t securityParams;

        securityParams.Type = SECURITY_TYPE;
        securityParams.Key = (signed char*)SECURITY_KEY;
        securityParams.KeyLen = strlen((const char *)securityParams.Key);

        ret = sl_WlanProfileAdd((signed char*)SSID_NAME, strlen(
                                    SSID_NAME), 0, &securityParams, NULL, 7, 0);
        if(ret < 0)
        {
            LOG_ERROR("failed to add profile %s\r\n", SSID_NAME);
        }
        else
        {
            LOG_INFO("profile added %s\r\n", SSID_NAME);
        }
    }

    Timer_stop(timer0);
    Timer_close(timer0);

    return(ret);
}

void mainThread(void * args){
    int32_t ret;
    mq_attr attr;
    Timer_Params params;
    UART_Handle uartHandle;
    struct msgQueue queueElement;
    MQTTClient_Handle mqttClientHandle;

    uartHandle = InitTerm();
    UART_control(uartHandle, UART_CMD_RXDISABLE, NULL);

    GPIO_init();
    SPI_init();
    Timer_init();

    //dht11
    GPIO_setConfig(dht11, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_STR_MED | GPIO_CFG_OUT_HIGH);


    /* Initialize SlNetSock layer with CC31xx/CC32xx interface */
    ret = SlNetIf_init(0);

    if(ret == 0)
    {
        ret = SlNetSock_init(0);
    }

    if(ret == 0)
    {
        ret = SlNetUtil_init(0);
    }

    if(ret == 0)
    {
        ret = SlNetIf_add(SLNETIF_ID_1, "CC32xx",
                          (const SlNetIf_Config_t *)&SlNetIfConfigWifi,
                          SLNET_IF_WIFI_PRIO);
    }

    if(0 != ret)
    {
        LOG_ERROR("Failed to initialize SlNetSock\n\r");
    }
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
    GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_OFF);
    GPIO_write(Board_GPIO_LED2, Board_GPIO_LED_OFF);

    GPIO_setCallback(Board_BUTTON0, pushButtonPublishHandler);
    GPIO_setCallback(Board_BUTTON1, pushButtonConnectionHandler);

    // configuring the timer to toggle an LED until the AP is connected
    Timer_Params_init(&params);
    params.period = 1000000;
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = (Timer_CallBackFxn)timerLEDCallback;

    timer0 = Timer_open(Board_TIMER0, &params);
    if(timer0 == NULL)
    {
        LOG_ERROR("failed to initialize timer\r\n");
        while(1)
        {
            ;
        }
    }

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct msgQueue);
    appQueue = mq_open("appQueue", O_CREAT, 0, &attr);
    if(((int)appQueue) <= 0)
    {
        while(1)
        {
            ;
        }
    }

    ret = WifiInit();
    if(ret < 0)
    {
        while(1)
        {
            ;
        }
    }

    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
    GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_OFF);
    GPIO_write(Board_GPIO_LED2, Board_GPIO_LED_OFF);

    params.period = 1500000;
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_ONESHOT_CALLBACK;
    params.timerCallback = (Timer_CallBackFxn)timerCallback;

    timer0 = Timer_open(Board_TIMER0, &params);
    if(timer0 == NULL)
    {
        LOG_ERROR("failed to initialize timer\r\n");
        while(1)
        {
            ;
        }
    }

    read_dht11();

MQTT_DEMO:

    ret = MQTT_IF_Init(mqttInitParams);
    if(ret < 0)
    {
        while(1)
        {
            ;
        }
    }

#ifdef MQTT_SECURE_CLIENT
    setTime();
#endif

    /*
     * In case a persistent session is being used, subscribe is called before connect so that the module
     * is aware of the topic callbacks the user is using. This is important because if the broker is holding
     * messages for the client, after CONNACK the client may receive the messages before the module is aware
     * of the topic callbacks. The user may still call subscribe after connect but have to be aware of this.
     */
    ret = MQTT_IF_Subscribe(mqttClientHandle, "Broker/To/cc32xx", MQTT_QOS_2,
                            BrokerCB);
    ret |= MQTT_IF_Subscribe(mqttClientHandle, "cc32xx/ToggleLED1", MQTT_QOS_2,
                             ToggleLED1CB);
    ret |= MQTT_IF_Subscribe(mqttClientHandle, "cc32xx/ToggleLED2", MQTT_QOS_2,
                             ToggleLED2CB);
    ret |= MQTT_IF_Subscribe(mqttClientHandle, "cc32xx/ToggleLED3", MQTT_QOS_2,
                             ToggleLED3CB);
    if(ret < 0)
    {
        while(1)
        {
            ;
        }
    }
    else
    {
        LOG_INFO("Subscribed to all topics successfully\r\n");
    }

    mqttClientHandle = MQTT_IF_Connect(mqttClientParams, mqttConnParams,
                                       MQTT_EventCallback);
    if(mqttClientHandle < 0)
    {
        while(1)
        {
            ;
        }
    }

    // wait for CONNACK
    while(connected == 0)
    {
        ;
    }

    GPIO_enableInt(Board_BUTTON0);

    while(1)
    {
        mq_receive(appQueue, (char*)&queueElement, sizeof(struct msgQueue),
                   NULL);

        if(queueElement.event == APP_MQTT_PUBLISH)
        {
            LOG_TRACE("APP_MQTT_PUBLISH\r\n");

            MQTT_IF_Publish(mqttClientHandle,
                            "cc32xx/ToggleLED1",
                            "LED 1 toggle\r\n",
                            strlen("LED 1 toggle\r\n"),
                            MQTT_QOS_2);

            GPIO_clearInt(Board_BUTTON0);
            GPIO_enableInt(Board_BUTTON0);
        }
        else if(queueElement.event == APP_MQTT_CON_TOGGLE)
        {
            LOG_TRACE("APP_MQTT_CON_TOGGLE %d\r\n", connected);

            if(connected)
            {
                ret = MQTT_IF_Disconnect(mqttClientHandle);
                if(ret >= 0)
                {
                    connected = 0;
                }
            }
            else
            {
                mqttClientHandle = MQTT_IF_Connect(mqttClientParams,
                                                   mqttConnParams,
                                                   MQTT_EventCallback);
                if((int)mqttClientHandle >= 0)
                {
                    connected = 1;
                }
            }
        }
        else if(queueElement.event == APP_MQTT_DEINIT)
        {
            break;
        }
        else if(queueElement.event == APP_BTN_HANDLER)
        {
            struct msgQueue queueElement;

            ret = detectLongPress();
            if(ret == 0)
            {
                LOG_TRACE("APP_BTN_HANDLER SHORT PRESS\r\n");
                queueElement.event = APP_MQTT_CON_TOGGLE;
            }
            else
            {
                LOG_TRACE("APP_BTN_HANDLER LONG PRESS\r\n");
                queueElement.event = APP_MQTT_DEINIT;
            }

            ret =
                mq_send(appQueue, (const char*)&queueElement,
                        sizeof(struct msgQueue), 0);
            if(ret < 0)
            {
                LOG_ERROR("msg queue send error %d", ret);
            }
        }
    }

    deinit = 1;
    if(connected)
    {
        MQTT_IF_Disconnect(mqttClientHandle);
    }
    MQTT_IF_Deinit();

    LOG_INFO(
        "looping the MQTT functionality of the example for demonstration purposes only\r\n");
    sleep(2);
    goto MQTT_DEMO;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
