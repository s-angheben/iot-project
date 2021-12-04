#ifndef DHT11_H_
#define DHT11_H_

#include "stdio.h"
#include "stdlib.h"
#include <time.h>

#define dht11_pin dht11
#define board_timer Board_TIMER1

void countHandler(int index);

void read_dht11(int* temperature, int* humidity);

void dht11_init();

#endif