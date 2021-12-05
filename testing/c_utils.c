#include "c_utils.h"

mqd_t appQueue;
config data_config;
info data_info;
char msg_info_data[MSGSIZE];
char msg_control[MSGSIZE];

void init_queue() {
    appQueue = mq_open("appQueue", O_CREAT | O_RDWR);
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

void sendinfo(info* p_info_data, config* p_config_data) {
    formatdata(msg_info_data, p_info_data, p_config_data);
    publish_mgs(msg_info_data, strlen(msg_info_data));
    printf("[-] pub :%s to %s\n", msg_info_data, INFO_TOPIC);
}

