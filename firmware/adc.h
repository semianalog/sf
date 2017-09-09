#ifndef ADC_H
#define ADC_H

#include <inttypes.h>

void init_adc(void);

void select_probe(void);
void select_pot(void);

int16_t adc_convert(void);

#endif // ADC_H
