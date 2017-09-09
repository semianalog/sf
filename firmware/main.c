#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "hwdefs.h"
#include "gpio.h"
#include "clocks_power.h"
#include "current_source.h"

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
    sei();
    PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;

    if (RST.STATUS & RST_PORF_bm) {
        RST.STATUS = RST_PORF_bm; // clear flag for later
        // If reset reason was POR, we should go straight into sleep
        enter_sleep();  // no return
    }

    start_fast_clock();
    start_current_source();
    set_current_range(RANGE_20MA);

    WRITE_PIN(IND_20mA, 1);
    set_charge_pump_dc(true);
    enter_run();

    struct button_state state = { true, false, 0 };

    // Initialize timer for audio sampling
    TCC5.CTRLB = TC45_WGMODE_NORMAL_gc;
    TCC5.CTRLE = 0;
    TCC5.PER = (F_CPU / (220uLL * 16uLL));
    TCC5.INTCTRLA = TC45_OVFINTLVL_MED_gc;
    TCC5.CTRLA = TC45_CLKSEL_DIV1_gc;
    TCC5.INTFLAGS = TC5_OVFIF_bm;

    for (;;) {
        button_cycle(&state);

        if (!READ_PIN(SW_OFF)) {
            enter_sleep();
        }
    }

    return 0;
}

static uint8_t sig_cca[] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 4.9, 9.0, 11.7, 12.7, 11.7, 9.0, 4.9 };
static uint8_t sig_ccb[] = {
    0, 4.9, 9.0, 11.7, 12.7, 11.7, 9.0, 4.9,
    0, 0, 0, 0, 0, 0, 0, 0 };

ISR(TCC5_OVF_vect)
{
    static uint8_t i = 0;

    uint8_t cca = sig_cca[i];
    uint8_t ccb = sig_ccb[i];

    WRITE_PIN(POT_EN, true);
    //TCC4.CTRLGSET = TC4_STOP_bm;
    TCC4.CCA = cca;
    TCC4.CCB = ccb;
    //TCC4.CTRLGCLR = TC4_STOP_bm;
    if (i == TC45_CMD_RESTART_gc)
        TCC4.CTRLGSET = TC45_CMD_RESTART_gc;
    WRITE_PIN(POT_EN, false);
    TCC5.INTFLAGS = TC5_OVFIF_bm;

    i = (i + 1) % 16;
}

static void button_cycle(struct button_state * state)
{
    const uint16_t debounce_top = 25000u;
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
