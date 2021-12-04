#include "c_utils.h"

mqd_t appQueue;
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


void formatdata(char msg[], info* info_data) {
    snprintf(msg, MSGSIZE, "{\"water\": %d, \"mode\":%d, \"umidity\":%d, \"temperature\":%d, \"light\":24.3}",
             info_data->water, info_data->mode, info_data->humidity, info_data->temperature);
}

void sendinfo() {
    formatdata(msg_info_data, &data_info);
    publish_mgs(msg_info_data, strlen(msg_info_data));  
    printf("[-] pub :%s to %s\n", msg_info_data, INFO_TOPIC);  
}