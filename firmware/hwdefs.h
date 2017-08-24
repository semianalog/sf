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
X(SW_RANGE,     2,  0,  false,  false,  PORT_OPC_PULLUP_gc | PORT_INVEN_bm) \
X(SW_OFF,       2,  1,  false,  false,  PORT_OPC_PULLUP_gc | PORT_INVEN_bm) \
X(VOLUME,       2,  2,  false,  false,  PORT_ISC_INPUT_DISABLE_gc) /* ADC: 10 */ \
X(POT_EN,       2,  3,  true,   false,  PORT_OPC_TOTEM_gc) \
X(DRIVE,        2,  4,  true,   false,  PORT_OPC_TOTEM_gc) /* OC5A */ \
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

#endif // HWDEFS_H
