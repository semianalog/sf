#include <avr/io.h>
#include <avr/interrupt.h>
#include "audio.h"
#include "hwdefs.h"

static const __flash int16_t SINE_TABLE[] = {
    0, 196, 362, 473, 512, 473, 362, 196, 0, -196, -362, -473, -512, -473, -362, -196
};

static volatile uint16_t samples_ph1[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static volatile uint16_t samples_ph2[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void init_audio(void)
{
    const uint16_t pwm_per = 128;
    HIRESC.CTRLA = HIRES_HREN_HRP4_gc;
    TMR_AUDIO_PWM.CTRLB = TC45_WGMODE_SINGLESLOPE_gc;
    TMR_AUDIO_PWM.CTRLE = TMR_AUDIO_PWM_CTRLE; // Enable outputs to pins
    TMR_AUDIO_PWM.PER = pwm_per << 2;
    CC_AUDIO_PWM_1 = 0;
    CC_AUDIO_PWM_2 = 0;
    CC_CHARGEPUMP = (pwm_per / 2) << 2;
    TMR_AUDIO_PWM.CTRLA = TC45_CLKSEL_DIV1_gc;

    //HIRESC.CTRLA = HIRES_HRPLUS_HRP4_gc;


    TMR_AUDIO_SAMPLE.CTRLB = TC45_WGMODE_NORMAL_gc;
    TMR_AUDIO_SAMPLE.CTRLE = 0;
    TMR_AUDIO_SAMPLE.INTCTRLA = TC45_OVFINTLVL_MED_gc;

    set_audio_freq(220);
    set_audio_volume(2);
    TMR_AUDIO_SAMPLE.CTRLA = TC45_CLKSEL_DIV1_gc;
    TMR_AUDIO_SAMPLE.INTFLAGS = TC5_OVFIF_bm;
}

void set_audio_freq(uint16_t freq)
{
    TMR_AUDIO_SAMPLE.PER = (F_CPU / (freq * 16uLL));
    TMR_AUDIO_SAMPLE.CTRLGSET = TC45_CMD_RESTART_gc;
}

void set_audio_volume(uint8_t vol)
{
    //TMR_AUDIO_SAMPLE.CTRLGSET = TC5_STOP_bm;

    for (uint8_t i = 0; i < sizeof(SINE_TABLE)/sizeof(SINE_TABLE[0]); ++i) {
        int16_t datapoint = SINE_TABLE[i];
        datapoint *= (int16_t) vol;
        datapoint /= 256;

        if (datapoint >= 0) {
            samples_ph1[i] = 0;
            samples_ph2[i] = datapoint;
        } else {
            samples_ph1[i] = -datapoint;
            samples_ph2[i] = 0;
        }
    }

    //TMR_AUDIO_SAMPLE.CTRLGCLR = TC5_STOP_bm;
}

ISR(VECT_AUDIO_SAMPLE)
{
    static uint8_t i = 0;

    uint16_t cca = samples_ph1[i];
    uint16_t ccb = samples_ph2[i];

    CC_AUDIO_PWM_1 = cca;
    CC_AUDIO_PWM_2 = ccb;

    if (i == TC45_CMD_RESTART_gc)
        TMR_AUDIO_PWM.CTRLGSET = TC45_CMD_RESTART_gc;

    i = (i + 1) % (sizeof(SINE_TABLE)/sizeof(SINE_TABLE[0]));
    TMR_AUDIO_SAMPLE.INTFLAGS = TC5_OVFIF_bm;
}
