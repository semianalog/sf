#include "clocks_power.h"
#include <avr/io.h>
#include <avr/xmega.h>

void start_fast_clock(void)
{
    const uint8_t en = OSC_RC32MEN_bm | OSC_RC32KEN_bm;
    const uint8_t rdy = OSC_RC32MRDY_bm | OSC_RC32KRDY_bm;

    OSC.CTRL |= en;
    while ((OSC.STATUS & rdy) != rdy);

    OSC.DFLLCTRL = OSC_RC32MCREF_RC32K_gc;
    DFLLRC32M.CTRL = DFLL_ENABLE_bm;

    _PROTECTED_WRITE(CLK.CTRL, CLK_SCLKSEL_RC32M_gc);

    // No need for any other clocks
    OSC.CTRL = en;
}

void enter_sleep(void)
{
    // TODO: Bias amplifier down and disable DAC
    // TODO: Disable charge pump, audio output, FET drive
    // TODO: Power down amplifier, volume pot, all LEDs
}
