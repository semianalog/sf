#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "hwdefs.h"
#include "gpio.h"
#include "clocks_power.h"
#include "current_source.h"
#include "audio.h"
#include "adc.h"

struct button_state {
    bool button_state_hold;
    bool pre_act;
    uint16_t count;
};

static void button_cycle(struct button_state * state);
static void button_action(void);

int main(void)
{
    _delay_ms(1);
    init_pins();

    if (RST.STATUS & RST_PORF_bm) {
        RST.STATUS = RST_PORF_bm; // clear flag for later
        // If reset reason was POR, we should go straight into sleep
        PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
        sei();
        enter_sleep();  // no return
    }

    start_fast_clock();
    start_current_source();
    set_current_range(RANGE_20MA);

    WRITE_PIN(IND_20mA, 1);
    set_charge_pump_dc(true);
    enter_run();
    init_audio();
    init_adc();

    // Disable audio sample inerrupt to allow debug PWM output
    //TMR_AUDIO_SAMPLE.INTCTRLA = 0;

    WRITE_PIN(POT_EN, true);
    select_pot();

    ADCA.EVCTRL = ADC_EVSEL_0_gc | ADC_EVACT_CH0_gc;
    PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
    sei();

    for (;;) {
        if (!READ_PIN(SW_OFF)) {
            enter_sleep();
        }
    }

    return 0;
}

static void button_cycle(struct button_state * state)
{
    const uint16_t debounce_top = 100u;
    bool button = READ_PIN(SW_RANGE);

    if (state->pre_act) {
        // Waiting for the button to change state
        if (button != state->button_state_hold) {
            state->button_state_hold = !state->button_state_hold;
            state->pre_act = false;
            state->count = 0;
        }
    } else {
        // Button must remain up/down for `debounce_top` cycles
        if (button == state->button_state_hold) {
            ++state->count;
            if (state->count == debounce_top) {
                state->pre_act = true;
                if (!button) {
                    // Button is down - act!
                    button_action();
                }
            }
        } else {
            state->count = 0;
        }
    }
}

static void button_action(void)
{
    step_current_range();
}

void adc_callback(int16_t signal_amp, uint8_t volume)
{
    volatile int16_t a = signal_amp;
    volatile uint8_t b = volume;

    static struct button_state state = { true, false, 0 };
    button_cycle(&state);

    // Output value direct to PWM for debug
    uint32_t pwmval = (uint32_t) TMR_AUDIO_PWM.PER * (uint32_t) signal_amp / 0xfffuLL;
    pwmval = 10;
    CC_AUDIO_PWM_1 = pwmval;
    CC_AUDIO_PWM_2 = pwmval;
    return;

    if (volume != UINT8_MAX) {
        set_audio_volume(volume / 256);
    }

    if (signal_amp != INT16_MAX) {
        set_audio_freq(signal_amp/2 + 220);
    }

    (void) a;
    (void) b;
}
