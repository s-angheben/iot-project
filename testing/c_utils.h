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

#define MSGSIZE 300

typedef struct {
    int mode;
    int water;
    int temperature;
    int humidity;
    int light;
} info;

typedef struct {
    int m3_humidity;
    int m3_delta;
} config;

void formatdata(char msg[], info* info_data, config* config_data);

void sendinfo(info* p_info_data, config* p_config_data);

void initialize_info(info* info_data);

void initialize_config(config* config_data);

#endif
