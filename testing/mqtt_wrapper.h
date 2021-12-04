#ifndef MQTT_WRAPPER_H
#define MQTT_WRAPPER_H

#include "MQTTClient.h"
#include <string.h>
#include <stdlib.h>
#include "c_utils.h"

#define ADDRESS     "tcp://192.168.1.6:1883"
#define CLIENTID_INFO       "PUB"
#define CLIENTID_CONTROL    "SUB"
#define INFO_TOPIC  "infoTopic"
#define CONTROL_TOPIC   "controlTopic"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L


void delivered(void *context, MQTTClient_deliveryToken dt);
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void connlost(void *context, char *cause);
int publish_mgs(char* text, int len);
void quit_mqtt_infoTopic();
void quit_mqtt_controlTopic();
void connect_mqtt_infoTopic();
void connect_mqtt_controlTopic();
int WifiInit();


#endif