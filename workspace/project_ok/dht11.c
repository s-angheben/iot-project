#include <dht11.h>

UInt32 last = 0;
UInt32 now = 0;
UInt32 savedate[45];
int counter = 1;
Timer_Handle timer1;
Timer_Params params1;



void countHandler(uint_least8_t index)
{
//    now = Clock_getTicks();
    now = Timer_getCount(timer1);
    savedate[counter] = (now - last);
    last = now;
    counter++;
}

void read_dht11(int* temperature, int* humidity){

    // pin low for 18ms
    GPIO_write(dht11_pin, LOW);
    Task_sleep(18000 / Clock_tickPeriod);

    // pin high
    GPIO_write(dht11_pin, HIGH);

    GPIO_setConfig(dht11_pin, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    GPIO_setCallback(dht11_pin, countHandler);
    // Enable interrupts
//    last = Clock_getTicks();

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

    //LOG_INFO("temperatura: %d, umidita: %d \n", *temperature, *humidity);
}

void dht11_init() {

    GPIO_setConfig(dht11_pin, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_STR_MED | GPIO_CFG_OUT_HIGH);

    Timer_Params_init(&params1);
    params1.period = 5000000;
    params1.periodUnits = Timer_PERIOD_US;
    params1.timerMode = Timer_FREE_RUNNING;
    params1.timerCallback = NULL;

    timer1 = Timer_open(board_timer, &params1);
    if (timer1 == NULL) {
            // Failed to initialized timer
        LOG_ERROR("failed to initialize timer\r\n");
        while (1) {}
    }

    if (Timer_start(timer1) == Timer_STATUS_ERROR) {
        // Failed to start timer
        LOG_ERROR("failed to initialize timer\r\n");
        while (1) {}
    }
    Timer_stop(timer1);
}


