#include "dht11.h"


void countHandler(int index)
{
//
}

void read_dht11(int* temperature, int* humidity){
    printf("[-] dht11 reading:\n\t { ");
    printf("dht11_pin low for 18ms, ");
/*    // pin low for 18ms
    GPIO_write(dht11_pin, LOW);
    Task_sleep(18000 / Clock_tickPeriod);
*/
    printf("dht11_pin high, ");
/*
    // pin high
    GPIO_write(dht11_pin, HIGH);

    GPIO_setConfig(dht11_pin, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    GPIO_setCallback(dht11_pin, countHandler);
    // Enable interrupts
//    last = Clock_getTicks();
*/
    printf("dht11_pin in, set timer, add handler, ");
/* 
    Timer_start(timer1);

    last = Timer_getCount(timer1);
    GPIO_enableInt(dht11_pin);

    // wait
    Task_sleep(25000 / Clock_tickPeriod);

    GPIO_disableInt(dht11_pin);
    Timer_stop(timer1);

    int j,i, c=2;
    uint8_t bytes[5];
    for (i = 0; i < 5; i++) bytes[i] = 0;
    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 8; j++)
        {
            bytes[i] |= ((savedate[c] > 4500) << (7 - j));
            c++;
        }
    }

    // the checksum will overflow automatically //
    uint8_t checkSum = 0;
    for (i = 0; i < (5 - 1); i++)  checkSum += bytes[i];
    if (checkSum != bytes[4]) while(1);

    *humidity    = bytes[0];
    *temperature = bytes[2];

    for(i=0; i<45; i++) {
        savedate[i] = 0;
    }
    counter = 0;


    GPIO_setConfig(dht11_pin, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_STR_MED | GPIO_CFG_OUT_HIGH);
*/
    printf("wait and calculate temp }\n");
    srand(time(0));
    *temperature = 5 + rand() % 30;
    *humidity = rand() % 100;
    printf("\t temperatura: %d, umidita: %d \n", *temperature, *humidity);
}

void dht11_init() {
    printf("[-] dht11 initialization\n");
}


