#include "current_source.h"
#include <avr/io.h>
#include "hwdefs.h"
#include "gpio.h"

enum current_ranges g_range = RANGE_20MA;

void start_current_source(void)
{
    TCD5.CTRLB = TC45_WGMODE_SINGLESLOPE_gc;
    TCD5.CTRLE = TC45_CCAMODE_COMP_gc;

    uint16_t const per = F_CPU / 1775uLL; // 2.5kHz
    TCD5.PER = per;
    TCD5.CCA = 650;
    TCD5.CTRLA = TC45_CLKSEL_DIV1_gc;
}

void stop_current_source(void)
{
    TCD5.CTRLE = 0;
    WRITE_PIN(DRIVE, false);
}

void step_current_range(void)
{
    enum current_ranges next = (g_range + 1) % NUM_RANGES;
    set_current_range(next);
}

void set_current_range(enum current_ranges range)
{
    WRITE_PIN(IND_20mA, false);
    WRITE_PIN(IND_100mA, false);
    WRITE_PIN(IND_1A, false);

    switch (range) {
    case RANGE_1A:
        TCD5.CCA = 650;
        WRITE_PIN(IND_1A, true);
        break;
    case RANGE_100MA:
        TCD5.CCA = 65;
        WRITE_PIN(IND_100mA, true);
        break;
    case RANGE_20MA:
        TCD5.CCA = 13;
        WRITE_PIN(IND_20mA, true);
        break;
    default:
        TCD5.CCA = 0;
    }

    g_range = range;
}
