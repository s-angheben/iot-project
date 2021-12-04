#ifndef C_UTILS_H_
#define C_UTILS_H_


#include <mqueue.h>
#include <fcntl.h>
#include "mqtt_wrapper.h"

//mqd_t appQueue;

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

#define MSGSIZE 100

typedef struct {
    int mode;
    int water;
    int temperature;
    int humidity;
    float light;
} info;

/*
info data_info;
char msg_info_data[MSGSIZE];
char msg_control[MSGSIZE];
*/

void formatdata(char msg[], info* info_data);

void sendinfo();

void initialize_info(info* info_data);

#endif