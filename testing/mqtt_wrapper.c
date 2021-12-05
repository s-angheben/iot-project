#include "mqtt_wrapper.h"

MQTTClient client_infoTopic, client_controlTopic;
MQTTClient_connectOptions conn_opts_infoTopic = MQTTClient_connectOptions_initializer;
MQTTClient_connectOptions conn_opts_controlTopic = MQTTClient_connectOptions_initializer;
MQTTClient_message pubmsg_infoTopic = MQTTClient_message_initializer;
MQTTClient_deliveryToken token;
volatile MQTTClient_deliveryToken deliveredtoken;


extern mqd_t appQueue;
extern char msg_control[MSGSIZE];


void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int WifiInit() {
    printf("[-] Wifi initialization\n");
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char* payloadptr;

    memset(msg_control,0,MSGSIZE);
    strncpy(msg_control, message->payload, message->payloadlen);
    struct msgQueue queueElement;  
    queueElement.event = APP_COMMAND_RECEIVED;
    
    mq_send(appQueue, (const char*)&queueElement, sizeof(struct msgQueue), 0);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int publish_mgs(char* text, int len) {
    pubmsg_infoTopic.payload = text;
    pubmsg_infoTopic.payloadlen = len;
    pubmsg_infoTopic.qos = QOS;
    pubmsg_infoTopic.retained = 0;
    MQTTClient_publishMessage(client_infoTopic, INFO_TOPIC, &pubmsg_infoTopic, &token);

    int rc = MQTTClient_waitForCompletion(client_infoTopic, token, TIMEOUT);

    return rc;
}


void quit_mqtt_infoTopic() {
    MQTTClient_disconnect(client_infoTopic, 10000);
    MQTTClient_destroy(&client_infoTopic);
}

void quit_mqtt_controlTopic() {
    MQTTClient_disconnect(client_controlTopic, 10000);
    MQTTClient_destroy(&client_controlTopic);
}

void connect_mqtt_infoTopic() {
    int rc;
    MQTTClient_create(&client_infoTopic, ADDRESS, CLIENTID_INFO,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts_infoTopic.keepAliveInterval = 20;
    conn_opts_infoTopic.cleansession = 1;

    if ((rc = MQTTClient_connect(client_infoTopic, &conn_opts_infoTopic)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
}

void connect_mqtt_controlTopic() {
    int ch, rc;
    MQTTClient_create(&client_controlTopic, ADDRESS, CLIENTID_CONTROL,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts_controlTopic.keepAliveInterval = 20;
    conn_opts_controlTopic.cleansession = 1;
    MQTTClient_setCallbacks(client_controlTopic, NULL, connlost, msgarrvd, delivered);
    if ((rc = MQTTClient_connect(client_controlTopic, &conn_opts_controlTopic)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }

    MQTTClient_subscribe(client_controlTopic, CONTROL_TOPIC, QOS);
}