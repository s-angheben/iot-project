#include<stdio.h>
#include<signal.h>
#include<sys/time.h> 

int handle_count=0;
void set_time(void)
{
   struct itimerval itv;
   itv.it_interval.tv_sec=5;//Load automatically and then respond every 10 seconds
   itv.it_interval.tv_usec=0;
   itv.it_value.tv_sec=5;//Time of First Timing
   itv.it_value.tv_usec=0;
   setitimer(ITIMER_REAL,&itv,NULL);
}

void alarm_handle(int sig)
{
   handle_count++;
   printf("have handle count is %d\n",handle_count);
}

void main(void)
{
   signal(SIGALRM,alarm_handle);
   set_time();
   
   while(1){

   }
   
   return;
}
