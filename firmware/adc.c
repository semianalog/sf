#include "adc.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include <stddef.h>

void init_adc(void)
{
    uint8_t cal0, cal1;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        NVM.CMD = NVM_CMD_READ_CALIB_ROW_gc;
        cal0 = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0));
        cal1 = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1));
        NVM.CMD = NVM_CMD_NO_OPERATION_gc;
    }

    ADCA.CALL = cal0;
    ADCA.CALH = cal1;

    // Signed mode, 12 bit
    ADCA.CTRLB = ADC_CONMODE_bm | ADC_RESOLUTION_12BIT_gc;
    // VREF = VCC / 1.6 (1.875V-ish)
    ADCA.REFCTRL = ADC_REFSEL_INTVCC_gc;
    ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc;

    ADCA.CH0.CTRL = ADC_CH_GAIN_1X_gc | ADC_CH_INPUTMODE_DIFFWGAINH_gc;

    ADCA.CTRLA = ADC_ENABLE_bm;
}

void select_probe(void)
{
    ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc | ADC_CH_MUXNEGH_PIN4_gc;
}

void select_pot(void)
{
    ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN10_gc | ADC_CH_MUXNEGH_GND_gc;
}

int16_t adc_convert(void)
{
    ADCA.INTFLAGS = ADC_CH0IF_bm;
    ADCA.CH0.CTRL |= ADC_CH_START_bm;

    while (!(ADCA.INTFLAGS & ADC_CH0IF_bm));

    return ADCA.CH0RES;
}

int16_t adc_convert_triggered(void)
{
    ADCA.INTFLAGS = ADC_CH0IF_bm;
    ADCA.EVCTRL = ADC_EVSEL_0_gc | ADC_EVACT_CH0_gc;

    while (!(ADCA.INTFLAGS & ADC_CH0IF_bm));
    int16_t res = ADCA.CH0RES;
    ADCA.EVCTRL = 0;

    return res;
}
