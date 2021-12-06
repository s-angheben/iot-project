#include <mqtt_if.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <mqueue.h>
#include <stdio.h>

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
#include "dht11.h"

#include "ti_drivers_config.h"

#define APPLICATION_NAME         "MQTT client"
#define APPLICATION_VERSION      "2.0.0"

#define SL_TASKSTACKSIZE            2048
#define SPAWN_TASK_PRIORITY         9

#define SLNET_IF_WIFI_PRIO          (5)

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


// info data
// "water\": 1, \"mode\":2, \"umidity\":60, \"temperature\":32.6, \"light\":24.3"
typedef struct {
    int mode;
    int water;
    int temperature;
    int humidity;
    float light;
} info;

typedef struct {
    int m3_humidity;
    int m3_delta;
} config;

info data_info;
config data_config;

#define MSGSIZE 300
char msg_info_data[MSGSIZE];
char msg_control[MSGSIZE];

MQTTClient_Handle mqttClientHandle;

char ClientId[13] = {"board0"};


void formatdata(char msg[], info* info_data, config* config_data) {
    memset(msg,0,MSGSIZE);
    if (config_data == NULL) {
        snprintf(msg, MSGSIZE, "{\"water\": %d, \"mode\":%d, \"umidity\":%d, \"temperature\":%d, \"light\":24.3}",
             info_data->water, info_data->mode, info_data->humidity, info_data->temperature);
    } else {
        snprintf(msg, MSGSIZE, "{\"water\": %d, \"mode\":%d, \"umidity\":%d, \"temperature\":%d, \"light\":24.3, \"m3_humidity\":%d, \"m3_delta\":%d}",
             info_data->water, info_data->mode, info_data->humidity, info_data->temperature, config_data->m3_humidity, config_data->m3_delta);
    }

}

enum
{
    APP_MQTT_PUBLISH,
    APP_MQTT_CON_TOGGLE,
    APP_MQTT_DEINIT,
    APP_BTN_HANDLER,
    APP_COMMAND_RECEIVED,
    APP_TIMER
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


// this timer callback toggles the LED once per second until the device connects to an AP
void timerLEDCallback(Timer_Handle myHandle)
{
    GPIO_toggle(Board_GPIO_LED0);
}

void alarm_hadler(Timer_Handle myHandle) {
    struct msgQueue queueElement;
    queueElement.event = APP_TIMER;

    mq_send(appQueue, (const char*)&queueElement, sizeof(struct msgQueue), 0);
}

/*
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
*/

void waterButtonHandler(uint_least8_t index)
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

void infoTopicCB(char* topic,
                 char* payload){
//   GPIO_toggle(Board_GPIO_LED0);
   LOG_INFO("TOPIC: %s \tPAYLOAD: %s\r\n", topic, payload);
}

void controlTopicCB(char* topic,
                    char* payload){
      memset(msg_control,0,MSGSIZE);
      strcpy(msg_control, payload);
//      GPIO_toggle(Board_GPIO_LED0);
      LOG_INFO("TOPIC: %s \tPAYLOAD: %s\r\n", topic, payload);
      int ret;
      struct msgQueue queueElement;

      queueElement.event = APP_COMMAND_RECEIVED;
      ret = mq_send(appQueue, (const char*)&queueElement, sizeof(struct msgQueue),
                     0);
      if(ret < 0)
      {
          LOG_ERROR("msg queue send error %d", ret);
      }
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

void sendinfo(info* p_info_data, config* p_config_data) {
    formatdata(msg_info_data, p_info_data, p_config_data);
    MQTT_IF_Publish(mqttClientHandle,
                                "infoTopic",
                                msg_info_data,
                                strlen(msg_info_data),
                                MQTT_QOS_2);
}

void initialize_info(info* info_data) {
    info_data->mode = 1;
    info_data->water = 0;
    info_data->temperature = 0;
    info_data->humidity = 0;
    info_data->light = 0;
}

void initialize_config(config* config_data) {
    config_data->m3_humidity = -1;
    config_data->m3_delta = 5;
}

void mainThread(void * args){
    int32_t ret;
    mq_attr attr;
    Timer_Params params;
    UART_Handle uartHandle;
    struct msgQueue queueElement;
    char* p_msg;
    int hysteresis_rising = 1;
    int humidity_curr;

    uartHandle = InitTerm();
    UART_control(uartHandle, UART_CMD_RXDISABLE, NULL);

    GPIO_init();
    SPI_init();
    Timer_init();

    dht11_init();

    initialize_info(&data_info);
    initialize_config(&data_config);

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

    GPIO_setCallback(Board_BUTTON0, waterButtonHandler);
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


    // set timer for sensor reading
    Timer_Params_init(&params);
    params.period = 3000000;
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = (Timer_CallBackFxn)alarm_hadler;

    timer0 = Timer_open(Board_TIMER0, &params);
    if(timer0 == NULL)
    {
        LOG_ERROR("failed to initialize timer\r\n");
        while(1)
        {
            ;
        }
    }
    Timer_start(timer0);

    read_dht11(&data_info.temperature, &data_info.humidity);
    humidity_curr = data_info.humidity;

MQTT_DEMO:

    ret = MQTT_IF_Init(mqttInitParams);
    if(ret < 0)
    {
        while(1)
        {
            ;
        }
    }

    /*
     * In case a persistent session is being used, subscribe is called before connect so that the module
     * is aware of the topic callbacks the user is using. This is important because if the broker is holding
     * messages for the client, after CONNACK the client may receive the messages before the module is aware
     * of the topic callbacks. The user may still call subscribe after connect but have to be aware of this.
     */
    ret = MQTT_IF_Subscribe(mqttClientHandle, "infoTopic", MQTT_QOS_2,
                            infoTopicCB);
    ret = MQTT_IF_Subscribe(mqttClientHandle, "controlTopic", MQTT_QOS_2,
                            controlTopicCB);
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
            LOG_TRACE("WATER BUTTON PUSHED");
            GPIO_toggle(Board_GPIO_LED0);
            if(data_info.water == 0) {
                data_info.water = 1;
            } else {
                data_info.water = 0;
            }

            read_dht11(&data_info.temperature, &data_info.humidity);
            sendinfo(&data_info, &data_config);

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

            ret = 0;
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
        else if(queueElement.event == APP_COMMAND_RECEIVED){
            LOG_TRACE("COMMAND RECEIVED");
            p_msg = msg_control;
            if (*p_msg == 'w') {
              p_msg++;
              if (*p_msg == '1') {
                GPIO_write(Board_GPIO_LED0, HIGH);
                data_info.water = 1;
              } else {
                GPIO_write(Board_GPIO_LED0, LOW);
                data_info.water = 0;
              }
            } else if (*p_msg == 'm') {
              p_msg++;
              if (*p_msg == '1') {
                data_info.mode = 1;
              } else if (*p_msg == '2') {
                data_info.mode = 2;
              } else if (*p_msg == '3') {
                if(data_config.m3_humidity != -1) {
                  data_info.mode = 3;
                }
              }
            } else if (*p_msg == 's') {
              p_msg++;
              if (*p_msg == 'h') {
                p_msg++;
                int tmp = atoi(p_msg);
                if (tmp > 0 && tmp < 99) {
                  data_config.m3_humidity = tmp;
                }
                printf("%d", data_config.m3_humidity);
              }
              if (*p_msg == 'd') {
                p_msg++;
                int tmp = atoi(p_msg);
                if (tmp > 2 && tmp < 15) {
                  data_config.m3_delta = tmp;
                }
                printf("%d", data_config.m3_humidity);
              }
            }
            sendinfo(&data_info, &data_config);
        } else if(queueElement.event == APP_TIMER) {
            LOG_TRACE("TIMER: sensor reading");
            read_dht11(&data_info.temperature, &humidity_curr);
            if (data_info.mode == 3) {                                    // se automatica
              if((data_info.humidity < data_config.m3_humidity + data_config.m3_delta) && hysteresis_rising == 1) {
                GPIO_write(Board_GPIO_LED0, HIGH);
                data_info.water = 1;
              }
              if((data_info.humidity >= data_config.m3_humidity + data_config.m3_delta) && hysteresis_rising == 1) {
                GPIO_write(Board_GPIO_LED0, LOW);
                data_info.water = 0;
                hysteresis_rising = 0;
              }
              if((data_info.humidity < data_config.m3_humidity - data_config.m3_delta) && hysteresis_rising == 0) {
                GPIO_write(Board_GPIO_LED0, HIGH);
                data_info.water = 1;
                hysteresis_rising = 1;
              }
            }
            data_info.humidity = humidity_curr;
            sendinfo(&data_info, &data_config);
        }

    }

    deinit = 1;
    if(connected)
    {
        MQTT_IF_Disconnect(mqttClientHandle);
    }
    MQTT_IF_Deinit();


    sleep(2);
    goto MQTT_DEMO;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
