#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"
#include "mqtt_wrapper.h"
#include "dht11.h"
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
   printf("[-] timer: update data\n");

   struct msgQueue queueElement;  
   queueElement.event = APP_TIMER;
    
   mq_send(appQueue, (const char*)&queueElement, sizeof(struct msgQueue), 0);

}


int main(int argc, char* argv[])
{
    int rc, ch;
    struct msgQueue queueElement;

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

    dht11_init();
    WifiInit();
    connect_mqtt_infoTopic();
    connect_mqtt_controlTopic();
    printf("[-] Subscribed to all topics successfully\r\n");

    read_dht11(&data_info.temperature, &data_info.humidity);

    int ret = 0;
    while(1) {
        ret = mq_receive(appQueue, (char*)&queueElement, sizeof(struct msgQueue),
                   NULL);
        if(ret == -1) exit(ret);

        if(queueElement.event == APP_COMMAND_RECEIVED) {
            printf("COMMAND RECEIVED: %s\n", msg_control);
            if ((char)msg_control[0] == 'w') {
                if ((char)msg_control[1] == '1') {
                    printf("\tset water_pin to HIGH\n");
                    data_info.water = 1;
              } else {
                printf("\tset water_pin to LOW\n");
                data_info.water = 0;
              }
            } else if ((char)msg_control[0] == 'm') {
              if ((char)msg_control[1] == '1') {
                data_info.mode = 1;
              } else if ((char)msg_control[1] == '2') {
                data_info.mode = 2;
              } else if ((char)msg_control[1] == '3') {
                data_info.mode = 3;
              }
            }
            sendinfo();
        } else if(queueElement.event == APP_TIMER) {
            read_dht11(&data_info.temperature, &data_info.humidity);
            sendinfo();
        }

    }

    quit_mqtt_controlTopic();
    quit_mqtt_infoTopic();

    return rc;
}
