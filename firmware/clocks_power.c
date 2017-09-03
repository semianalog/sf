#include "clocks_power.h"
#include "gpio.h"
#include <avr/io.h>
#include <avr/xmega.h>
#include <avr/sleep.h>

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

void set_charge_pump_dc(bool run)
{
    _Static_assert(BOOST_IN_1_PORT == BOOST_IN_2_PORT, "Boost input pins must be on same port");
    _Static_assert(BOOST_IN_1_PORT == BOOST_IN_3_PORT, "Boost input pins must be on same port");
    VPORT_t * const boost_vport = VPORT_FOR_PIN(BOOST_IN_1);
    uint8_t const boost_mask = (1 << BOOST_IN_1_PIN) | (1 << BOOST_IN_2_PIN) | (1 << BOOST_IN_3_PIN);

    if (run) {
        boost_vport->OUT |= boost_mask;
    } else {
        boost_vport->OUT &= ~boost_mask;
    }
}

void enter_sleep(void)
{
    // TODO: Bias amplifier down and disable DAC
    // TODO: Disable charge pump, audio output, FET drive
    set_charge_pump_dc(false);
    WRITE_PIN(DRIVE, 0);

    // TODO: Power down amplifier, volume pot, all LEDs
    WRITE_PIN(POT_EN, 0);
    WRITE_PIN(IND_1A, 0);
    WRITE_PIN(IND_100mA, 0);
    WRITE_PIN(IND_20mA, 0);

    PR.PRGEN = PR_XCL_bm | PR_RTC_bm | PR_EVSYS_bm | PR_EDMA_bm;
    PR.PRPA = PR_DAC_bm | PR_ADC_bm | PR_AC_bm;
    PR.PRPC = PR_TWI_bm | PR_USART0_bm | PR_SPI_bm | PR_HIRES_bm | PR_TC5_bm | PR_TC4_bm;
    PR.PRPD = PR_TWI_bm | PR_USART0_bm | PR_SPI_bm | PR_HIRES_bm | PR_TC5_bm | PR_TC4_bm;

    set_sleep_mode(SLEEP_SMODE_PDOWN_gc);
    sleep_enable();
    sleep_cpu();
    for (;;);
}

void enter_run(void)
{
    PR.PRGEN = PR_XCL_bm | PR_RTC_bm;
    PR.PRPC = PR_TWI_bm | PR_USART0_bm | PR_SPI_bm | PR_HIRES_bm;
    PR.PRPD = PR_TWI_bm | PR_SPI_bm | PR_HIRES_bm;

    start_timers();
}

void start_timers(void)
{
    // TCC4: audio output and charge pump drive
    TCC4.CTRLB = TC45_WGMODE_SINGLESLOPE_gc;
    TCC4.CTRLE = TC45_CCAMODE_COMP_gc | TC45_CCBMODE_COMP_gc | TC45_CCCMODE_COMP_gc;
    uint16_t const per = F_CPU / 100000uLL; // 100kHz
    TCC4.PER = per;
    TCC4.CCA = per / 2;
    TCC4.CCB = per / 2;
    TCC4.CCC = per / 2;
    TCC4.CTRLA = TC45_CLKSEL_DIV1_gc;
}
