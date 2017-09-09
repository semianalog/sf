#ifndef HWDEFS_H
#define HWDEFS_H

#include <stdbool.h>

/* This file contains a simple set of definitions extracted from the schematic.
 * No copyright is claimed as it's just a list of facts; however, if you must
 * treat it as being under copyright and having a license, you can use either
 * the license in LICENSE-hw (CC-BY-SA 4.0) or the license in LICENSE-fw (MIT)
 * as you prefer.
 */

/******************************************************************************
 * PERIPHERAL MAPPING
 *
 * ANALOG
 * ----------------------------------------------------------------------------
 * ADC1, positive differential input: amplified probe signal
 * ADC4, negative differential input: probe amplifier bias level
 * ADC2, comparator input 2: current sense voltage
 * ADC10: volume pot
 * DAC1: probe amplifier bias level
 *
 * TIMERS
 * ----------------------------------------------------------------------------
 * TCC4:   (Fper/127 = 63kHz) audio output and charge pump
 *   OC4A: audio output, polarity 1
 *   OC4B: audio output, polarity 2
 *   OC4C: charge pump
 *
 * TCC5:   (variable freq) audio output DMA timer
 *
 * TCD5:   (1775 Hz) current source drive and ADC synch
 *   OC5A: high-side switch (IO inverted)
 */

/******************************************************************************
 * GPIO DEFINITIONS
 *
 * Ports A, C, D, R are numbered as 0, 1, 2, 3. This simplifies interfacing with
 * the VPORT.
 */

// NET
//              PORT
//                  PIN
//                      DIR
//                              DEFAULT
//                                      PINCTRL
#define PIN_TABLE \
X(PA0,          0,  0,  false,  false,  PORT_OPC_PULLUP_gc) /* unused */ \
X(SIGNAL,       0,  1,  false,  false,  PORT_ISC_INPUT_DISABLE_gc) /* ADC: 1 */ \
X(ISENSE,       0,  2,  false,  false,  PORT_ISC_INPUT_DISABLE_gc) /* Comparator */ \
X(BIAS_OUT,     0,  3,  false,  false,  PORT_ISC_INPUT_DISABLE_gc) /* DAC: 1 */ \
X(BIAS_IN,      0,  4,  false,  false,  PORT_ISC_INPUT_DISABLE_gc) /* ADC: 4 */ \
X(IND_1A,       0,  5,  true,   false,  PORT_OPC_TOTEM_gc) \
X(IND_100mA,    0,  6,  true,   false,  PORT_OPC_TOTEM_gc) \
X(IND_20mA,     0,  7,  true,   false,  PORT_OPC_TOTEM_gc) \
X(AUDIO_A,      1,  0,  true,   false,  PORT_OPC_TOTEM_gc) /* OC4A */ \
X(AUDIO_B,      1,  1,  true,   false,  PORT_OPC_TOTEM_gc) /* OC4B */ \
X(AUDIO_DET,    1,  2,  false,  false,  PORT_OPC_PULLUP_gc) \
X(BOOST_IN_1,   1,  3,  true,   false,  PORT_OPC_TOTEM_gc) \
X(BOOST_IN_2,   1,  4,  true,   false,  PORT_OPC_TOTEM_gc) \
X(BOOST_IN_3,   1,  5,  true,   false,  PORT_OPC_TOTEM_gc) \
X(BOOST_SW,     1,  6,  true,   false,  PORT_OPC_TOTEM_gc) /* OC4C */ \
X(GAIN,         1,  7,  true,   false,  PORT_OPC_TOTEM_gc) \
X(SW_RANGE,     2,  0,  false,  false,  PORT_OPC_PULLUP_gc | PORT_ISC_LEVEL_gc) \
X(SW_OFF,       2,  1,  false,  false,  PORT_OPC_PULLUP_gc) \
X(VOLUME,       2,  2,  false,  false,  PORT_ISC_INPUT_DISABLE_gc) /* ADC: 10 */ \
X(POT_EN,       2,  3,  true,   false,  PORT_OPC_TOTEM_gc) \
X(DRIVE,        2,  4,  true,   false,  PORT_OPC_TOTEM_gc | PORT_INVEN_bm) /* OC5A */ \
X(ISENSE_PASS,  2,  5,  false,  false,  PORT_ISC_INPUT_DISABLE_gc) \
X(RX,           2,  6,  false,  false,  PORT_OPC_PULLUP_gc) /* USART */ \
X(TX,           2,  7,  true,   true,   PORT_OPC_TOTEM_gc) /* USART */ \
X(PR0,          3,  0,  false,  false,  PORT_OPC_PULLUP_gc) /* unused */ \
X(PR1,          3,  1,  false,  false,  PORT_OPC_PULLUP_gc) /* unused */ \

/******************************************************************************
 * GPIO WHOLE_PORT DEFINITIONS
 */

#define PORTC_REMAP_VAL (PORT_TC4C_bm)
#define PORTD_REMAP_VAL (PORT_USART0_bm)

/******************************************************************************
 * ANALOG DEFINITIONS
 */

// Inductance of the current source buck converter, H
#define DRIVE_FILTER_H              (100e-6)

// Resistance of the current source sense resistor, ohms
#define CURRENT_SENSE_R             (1.0)

// Maximum Vf of clamping diode at 1A, volts
#define CLAMP_DIODE_VF              (0.5)

// Gain of amplifier stage 1, gain switch off, dB
#define AMP_GAIN_1_DB               (39.3)

// Gain of both amplifier stages together, gain switch off, dB
#define AMP_GAIN_2_DB               (78.5)

// Gain of amplifier stage 1, gain switch on, dB
#define AMP_GAIN_1_LO_DB            (19.4)

// Gain of both amplifier stages together, gain switch on, dB
#define AMP_GAIN_2_LO_DB            (58.6)

// BEWARE: Enabling the low-gain switch widens the bandwidth, from
// f_ch = 10.2 kHz to f_ch = 112 kHz.

// Center frequency of amplifier, Hz
#define AMP_FC_HZ                   (1.775e3)

// Proper frontend bias voltage, V. Use this as base for "BIAS" DAC output
#define FRONTEND_BIAS_V             (1.5)

// Nominal VCC. This is unregulated, so when accuracy is needed, measure
// with ADC.
#define VCC_V                       (3.0)

// Capacitance in the frontend supply charge pump, F. Same capacitance is used
// for drive coupling as for output filtering.
#define CHARGE_PUMP_CAP_F           (2.2e-6)

#endif // HWDEFS_H
