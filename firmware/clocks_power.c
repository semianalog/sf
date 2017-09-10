#include "clocks_power.h"
#include "current_source.h"
#include "gpio.h"
#include <avr/io.h>
#include <avr/xmega.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

void start_fast_clock(void)
{
    const uint8_t en = OSC_RC32MEN_bm | OSC_RC32KEN_bm;
    const uint8_t rdy = OSC_RC32MRDY_bm | OSC_RC32KRDY_bm;

    OSC.CTRL |= en;
    while ((OSC.STATUS & rdy) != rdy);

    OSC.DFLLCTRL = OSC_RC32MCREF_RC32K_gc;
    DFLLRC32M.CTRL = DFLL_ENABLE_bm;

    OSC.PLLCTRL = OSC_PLLSRC_RC32M_gc /* div 4 */ | 16;
    OSC.CTRL |= OSC_PLLEN_bm;

    while (!(OSC.STATUS & OSC_PLLRDY_bm));

    _PROTECTED_WRITE(CLK.PSCTRL, CLK_PSADIV_1_gc | CLK_PSBCDIV_2_2_gc);
    _PROTECTED_WRITE(CLK.CTRL, CLK_SCLKSEL_PLL_gc);
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
    stop_dac();
    stop_timers();
    set_charge_pump_dc(false);
    stop_current_source();

    // TODO: Power down amplifier, volume pot, all LEDs
    WRITE_PIN(POT_EN, 0);
    WRITE_PIN(IND_1A, 0);
    WRITE_PIN(IND_100mA, 0);
    WRITE_PIN(IND_20mA, 0);

    PR.PRGEN = PR_XCL_bm | PR_RTC_bm | PR_EVSYS_bm | PR_EDMA_bm;
    PR.PRPA = PR_DAC_bm | PR_ADC_bm | PR_AC_bm;
    PR.PRPC = PR_TWI_bm | PR_USART0_bm | PR_SPI_bm | PR_HIRES_bm | PR_TC5_bm | PR_TC4_bm;
    PR.PRPD = PR_TWI_bm | PR_USART0_bm | PR_SPI_bm | PR_HIRES_bm | PR_TC5_bm | PR_TC4_bm;

    // Enable the pin change interrupt on the ON switch
    PORT_FOR_PIN(SW_RANGE)->INTMASK = 1 << SW_RANGE_PIN;
    PORT_FOR_PIN(SW_RANGE)->INTCTRL = PORT_INTLVL_HI_gc;

    set_sleep_mode(SLEEP_SMODE_PDOWN_gc);
    sleep_enable();
    sleep_cpu();

    for (;;);
}

ISR(PORTD_INT_vect)
{
    _PROTECTED_WRITE(RST.CTRL, RST_SWRST_bm);
    for(;;);
}

void enter_run(void)
{
    PR.PRGEN = PR_XCL_bm | PR_RTC_bm;
    PR.PRPC = PR_TWI_bm | PR_USART0_bm | PR_SPI_bm;
    PR.PRPD = PR_TWI_bm | PR_SPI_bm | PR_HIRES_bm;

    start_dac();
}

void stop_timers(void)
{
    TCC5.CTRLE = 0;
    TCC5.CTRLA = 0;
    TCC4.CTRLE = 0;
    TCC4.CTRLA = 0;
    TCD5.CTRLE = 0;
    TCD5.CTRLA = 0;
}

void start_dac(void)
{
    DACA.CTRLB = DAC_CHSEL_SINGLE1_gc;
    DACA.CTRLC = DAC_REFSEL_AVCC_gc;
    DACA.CTRLA = DAC_CH1EN_bm | DAC_LPMODE_bm | DAC_ENABLE_bm;
    DACA.CH1DATA = 0x7ff;
}

void stop_dac(void)
{
    DACA.CTRLA = 0;
}
