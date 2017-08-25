#ifndef CLOCKS_POWER_H
#define CLOCKS_POWER_H

/// Start the 32 MHz oscillator, referenced to the calibrated 32 kHz oscillator,
/// and select it.
void start_fast_clock(void);

/// Enter sleep mode, disabling internal and external peripherals as necessary.
void enter_sleep(void);

#endif // CLOCKS_POWER_H
