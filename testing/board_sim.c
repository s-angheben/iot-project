#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"
#include "mqtt_wrapper.h"
#include "dht11.h"
#include "photoresistor.c"
#include "c_utils.h"
#include <mqueue.h>
#include <errno.h>
#include<stdio.h>
#include<signal.h>
#include<sys/time.h>


extern int errno;

extern mqd_t appQueue;
extern char msg_info_data[MSGSIZE];
extern char msg_control[MSGSIZE];
extern info data_info;
extern config data_config;

struct mq_attr attr;

void set_time(void)  // timer every 2 seconds
{
   struct itimerval itv;
   itv.it_interval.tv_sec=5;
   itv.it_interval.tv_usec=0;
   itv.it_value.tv_sec=5;//Time of First Timing
   itv.it_value.tv_usec=0;
   setitimer(ITIMER_REAL,&itv,NULL);
}

void alarm_handle(int sig)
{
   printf("\n[-] timer: update data\n");

   struct msgQueue queueElement;
   queueElement.event = APP_TIMER;

   mq_send(appQueue, (const char*)&queueElement, sizeof(struct msgQueue), 0);

}


int main(int argc, char* argv[])
{
    int rc, ch;
    char* p_msg;
    struct msgQueue queueElement;
    int hysteresis_rising = 1;
    int humidity_curr;

    signal(SIGALRM,alarm_handle);
    set_time();

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct msgQueue);
    mqd_t res;
    res = appQueue = mq_open("/appQueue", O_RDWR | O_CREAT, 0777, &attr);
    if(res == -1)  {
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error creating queue: ");
        exit(4);
    }

    initialize_info(&data_info);
    initialize_config(&data_config);

    dht11_init();
    WifiInit();
    connect_mqtt_infoTopic();
    connect_mqtt_controlTopic();
    printf("[-] Subscribed to all topics successfully\r\n");

    read_dht11(&data_info.temperature, &data_info.humidity);
    read_photo_resistor(&data_info.light);

    humidity_curr = data_info.humidity;

    int ret = 0;
    while(1) {
        ret = mq_receive(appQueue, (char*)&queueElement, sizeof(struct msgQueue),
                   NULL);
        if(ret == -1) exit(ret);

        if(queueElement.event == APP_COMMAND_RECEIVED) {
            printf("\nCOMMAND RECEIVED: %s\n", msg_control);
            p_msg = msg_control;
            if (*p_msg == 'w') {
              p_msg++;
              if (*p_msg == '1') {
                printf("\tset water_pin to HIGH\n");
                data_info.water = 1;
              } else {
                printf("\tset water_pin to LOW\n");
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
            read_dht11(&data_info.temperature, &humidity_curr);
	    read_photo_resistor(&data_info.light);
            if (data_info.mode == 3) {                                    // se automatica
              if((data_info.humidity < data_config.m3_humidity + data_config.m3_delta) && hysteresis_rising == 1) {
                printf("\tset water_pin to HIGH\n");
                data_info.water = 1;
              }
              if((data_info.humidity >= data_config.m3_humidity + data_config.m3_delta) && hysteresis_rising == 1) {
                printf("\tset water_pin to LOW\n");
                data_info.water = 0;
                hysteresis_rising = 0;
              }
              if((data_info.humidity < data_config.m3_humidity - data_config.m3_delta) && hysteresis_rising == 0) {
                printf("\tset water_pin to HIGH\n");
                data_info.water = 1;
                hysteresis_rising = 1;
              }
            }
            data_info.humidity = humidity_curr;
            sendinfo(&data_info, &data_config);
        }

    }

    quit_mqtt_controlTopic();
    quit_mqtt_infoTopic();

    return rc;
}
