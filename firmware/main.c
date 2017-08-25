#include <avr/io.h>
#include "hwdefs.h"
#include "gpio.h"
#include "clocks_power.h"

int main(void)
{
    start_fast_clock();

    init_pins();
    return 0;
}
