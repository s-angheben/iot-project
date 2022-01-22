#ifndef PHOTORESISTOR_H_
#define PHOTORESISTOR_H_

#include <ti/drivers/ADC.h>
#include <stdint.h>
#include <stddef.h>
#include "ti_drivers_config.h"

#define adc_photo_pin CONFIG_ADC_0


void read_photo_resistor(uint16_t* value);

#endif
