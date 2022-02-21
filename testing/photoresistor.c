#include "photoresistor.h"

void read_photo_resistor(int* light_value) {
	printf("[-] photoresistor reading:\n\t { ");
	printf("ADC reading, ADC_convert }\n");
	srand(time(0));
        *light_value = rand() % 100;
	printf("\t light: %d\n", *light_value);
}
