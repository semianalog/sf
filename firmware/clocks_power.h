#ifndef CLOCKS_POWER_H
#define CLOCKS_POWER_H

#include <stdbool.h>

/// Start the 32 MHz oscillator, referenced to the calibrated 32 kHz oscillator,
/// and select it.
void start_fast_clock(void);

/// Enable or disable fixed charge pump input
void set_charge_pump_dc(bool run);

/// Enter sleep mode, disabling internal and external peripherals as necessary.
void enter_sleep(void);

/// Enter run mode, initializing power management as necessary.
void enter_run(void);

#endif // CLOCKS_POWER_H
