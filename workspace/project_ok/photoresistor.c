#include <photoresistor.h>

void read_photo_resistor(uint16_t* value) {
    ADC_Handle   adc;
    ADC_Params   params;
    int_fast16_t res;

    uint16_t adcValue0;

    ADC_Params_init(&params);
    adc = ADC_open(adc_photo_pin, &params);

    if (adc == NULL) {
        while (1);
    }

    res = ADC_convert(adc, &adcValue0);

    if (res != ADC_STATUS_SUCCESS) {
        while (1);
    }

//    uint32_t adcValue0MicroVolt = ADC_convertRawToMicroVolts(adc, adcValue0);

    ADC_close(adc);
    *value = adcValue0;
}

