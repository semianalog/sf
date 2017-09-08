#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "hwdefs.h"
#include "gpio.h"
#include "clocks_power.h"

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

    WRITE_PIN(IND_1A, 1);
    set_charge_pump_dc(true);
    enter_run();

    for (;;) {
        if (!READ_PIN(SW_OFF)) {
            enter_sleep();
        }
    }

    return 0;
}
