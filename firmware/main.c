#include <avr/io.h>
#include <avr/interrupt.h>
#include "hwdefs.h"
#include "gpio.h"
#include "clocks_power.h"

int main(void)
{
    start_fast_clock();

    init_pins();
    WRITE_PIN(IND_1A, 1);
    set_charge_pump_dc(true);
    enter_run();
    sei();

    for (;;) {
        if (READ_PIN(SW_OFF)) {
            enter_sleep();
        }
    }

    return 0;
}
