#ifndef ADC_H
#define ADC_H

#include <inttypes.h>

void init_adc(void);

void select_probe(void);
void select_pot(void);

int16_t adc_convert(void);

// Configure the ADC to convert on the next event system trigger,
// generated by TCD5.CCB
int16_t adc_convert_triggered(void);

#endif // ADC_H
