#include "adc.h"
#include "hwdefs.h"
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

    ADCA.CH0.INTCTRL = ADC_CH_INTMODE_COMPLETE_gc | ADC_CH_INTLVL_LO_gc;
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

enum samp_state {
    SAMPLE_PH0,
    SAMPLE_PH90,
    SAMPLE_VOLPOT,
};

static int16_t process_samples(
        int16_t const * samples_ph0,
        int16_t const * samples_ph90,
        uint8_t n_samples)
{
    int32_t avg_ph0 = 0;
    int32_t avg_ph90 = 0;

    for (uint8_t i = 0; i < n_samples; ++i) {
        avg_ph0 += samples_ph0[i];
        avg_ph90 += samples_ph90[i];
    }

    avg_ph0 /= n_samples;
    avg_ph90 /= n_samples;

    avg_ph0 *= avg_ph0;
    avg_ph90 *= avg_ph90;

    return (avg_ph0 + avg_ph90) / 0xfffLL;
}

static uint8_t process_volume(
        int16_t const * samples,
        uint8_t n_samples)
{
    int16_t avg_sample = 0;

    for (uint8_t i = 0; i < n_samples; ++i) {
        avg_sample += samples[i];
    }

    avg_sample /= n_samples;

    return (avg_sample < 0) ? 0 : avg_sample / 256;
}

ISR(ADCA_CH0_vect)
{
    // Quadrature sampling to remove phase offset. To get amplitude of the
    // actual signal, use (sample_ph0 ^2 + sample_ph90 ^2)
    const uint16_t sample_offset_ph0 = 1000;
    const uint16_t sample_offset_ph90 = 1000 + (F_CPU / AMP_FC_HZ) / 4;
    const uint8_t total_samples = 16;

    static int16_t sample_buf_ph0[16];
    static int16_t sample_buf_ph90[16];
    static uint8_t n_samp = 0;
    static enum samp_state samp_state = SAMPLE_PH0;

    int16_t res = ADCA.CH0RES;

    switch (samp_state) {
    case SAMPLE_PH0:
        sample_buf_ph0[n_samp] = res;
        samp_state = SAMPLE_PH90;
        TCD5.CCB = sample_offset_ph90;
        break;
    case SAMPLE_PH90:
        sample_buf_ph90[n_samp] = res;
        n_samp = (n_samp + 1) % total_samples;

        if (n_samp == 0) {
            int16_t level = process_samples(sample_buf_ph0, sample_buf_ph90, total_samples);
            adc_callback(level, UINT8_MAX);
            samp_state = SAMPLE_VOLPOT;
            TCD5.CCB = 0;
        } else {
            samp_state = SAMPLE_PH0;
            TCD5.CCB = sample_offset_ph0;
        }
        break;
    case SAMPLE_VOLPOT:
        sample_buf_ph0[n_samp] = res;
        n_samp = (n_samp + 1) % total_samples;

        if (n_samp == 0) {
            uint8_t level = process_volume(sample_buf_ph0, total_samples);
            adc_callback(INT16_MAX, level);
            samp_state = SAMPLE_PH0;
            TCD5.CCB = sample_offset_ph0;
        }
        break;
    }
}
