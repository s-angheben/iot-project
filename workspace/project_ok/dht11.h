#ifndef DHT11_H_
#define DHT11_H_

#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/GPIO.h>
#include <ti_drivers_config.h>
#include "debug_if.h"


#define dht11_pin dht11
#define board_timer Board_TIMER1

void countHandler(uint_least8_t index);

void read_dht11(int* temperature, int* humidity);

void dht11_init();

#endif
