#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "hwdefs.h"
#include "gpio.h"
#include "clocks_power.h"
#include "current_source.h"
#include "audio.h"

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
    init_audio();
    enter_run();

    struct button_state state = { true, false, 0 };

    for (;;) {
        button_cycle(&state);

        if (!READ_PIN(SW_OFF)) {
            enter_sleep();
        }
    }

    return 0;
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
