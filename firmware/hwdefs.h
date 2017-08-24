#ifndef HWDEFS_H
#define HWDEFS_H

/* This file contains a simple set of definitions extracted from the schematic.
 * No copyright is claimed as it's just a list of facts; however, if you must
 * treat it as being under copyright and having a license, you can use either
 * the license in LICENSE-hw (CC-BY-SA 4.0) or the license in LICENSE-fw (MIT)
 * as you prefer.
 */

#define GPIO_OUTPUT 0x01    // GPIO direction is OUTPUT (default: INPUT)
#define GPIO_PULLUP 0x02    // GPIO pullup is active
#define GPIO_INVERT 0x04    // GPIO IO is inverted.
                            //   On XMEGA this affects peripherals.
#define GPIO_FAST   0x08    // GPIO slew rate is not limited.
                            //   Warning: SRL can only be set for a whole port.
                            //   If all pins in a port do not have the same
                            //   setting, port will be set FAST.
#define GPIO_ANALOG 0x10    // Disable digital input buffer
#define GPIO_DEF_HI 0x20    // Default output value is 1 (otherwise, 0)

/******************************************************************************
 * GPIO DEFINITIONS
 *
 * Ports A, C, D, R are numbered as 0, 1, 2, 3. This simplifies interfacing with
 * the VPORT.
 */

// Net: SIGNAL      - output of probe amplifier. Mode: ADC, sampled
//                    differentially relative to BIAS_IN.
#define SIGNAL_PORT     0
#define SIGNAL_PIN      1
#define SIGNAL_MODE     GPIO_ANALOG
#define SIGNAL_ADC      1

// Net: ISENSE      - current sense resistor. Mode: comparator
#define ISENSE_PORT     0
#define ISENSE_PIN      2
#define ISENSE_MODE     GPIO_ANALOG

// Net: BIAS_OUT    - output for probe amplifier bias. Mode: DAC
#define BIAS_OUT_PORT   0
#define BIAS_OUT_PIN    3
#define BIAS_OUT_MODE   GPIO_ANALOG
#define BIAS_OUT_DAC    1

// Net: BIAS_IN     - feedback of probe amplifier bias. Mode: ADC, negative
//                    differential input for SIGNAL
#define BIAS_IN_PORT    0
#define BIAS_IN_PIN     4
#define BIAS_IN_MODE    GPIO_ANALOG
#define BIAS_IN_ADC     4

// Net: IND_1A      - 1A range LED
#define IND_1A_PORT     0
#define IND_1A_PIN      5
#define IND_1A_MODE     GPIO_OUTPUT

// Net: IND_100mA   - 100mA range LED
#define IND_100mA_PORT  0
#define IND_100mA_PIN   6
#define IND_100mA_MODE  GPIO_OUTPUT

// Net: IND_20mA    - 20mA range LED
#define IND_20mA_PORT   0
#define IND_20mA_PIN    7
#define IND_20mA_MODE   GPIO_OUTPUT

// Net: AUDIO_A     - First audio output. Mode: TC4
#define AUDIO_A_PORT    1
#define AUDIO_A_PIN     0
#define AUDIO_A_MODE    GPIO_OUTPUT
#define AUDIO_A_TC4     0   // OC4A

// Net: AUDIO_B     - Second audio output. Mode: TC4
#define AUDIO_B_PORT    1
#define AUDIO_B_PIN     1
#define AUDIO_B_MODE    GPIO_OUTPUT
#define AUDIO_B_TC4     1   // OC4B

// Net: AUDIO_DET   - Headphone detect input
#define AUDIO_DET_PORT  1
#define AUDIO_DET_PIN   2
#define AUDIO_DET_MODE  GPIO_PULLUP

// Net: BOOST_IN_1  - Boost converter power input. Paralleled for low resistance
#define BOOST_IN_1_PORT 1
#define BOOST_IN_1_PIN  3
#define BOOST_IN_1_MODE GPIO_OUTPUT

// Net: BOOST_IN_2  - Boost converter power input. Paralleled for low resistance
#define BOOST_IN_2_PORT 1
#define BOOST_IN_2_PIN  4
#define BOOST_IN_2_MODE GPIO_OUTPUT

// Net: BOOST_IN_3  - Boost converter power input. Paralleled for low resistance
#define BOOST_IN_3_PORT 1
#define BOOST_IN_3_PIN  5
#define BOOST_IN_3_MODE GPIO_OUTPUT

// Net: BOOST_SW    - Boost converter switching node. Mode: TC4
#define BOOST_SW_PORT   1
#define BOOST_SW_PIN    6
#define BOOST_SW_MODE   GPIO_OUTPUT
#define BOOST_SW_TC4    2   // OC4C

// Net: GAIN        - Gain switch
#define GAIN_PORT       1
#define GAIN_PIN        7
#define GAIN_MODE       GPIO_OUTPUT

// Net: SW_RANGE    - Pushbutton: RANGE/ON
#define SW_RANGE_PORT   2
#define SW_RANGE_PIN    0
#define SW_RANGE_MODE   (GPIO_PULLUP | GPIO_INVERT)

// Net SW_OFF       - Pushbutton: OFF
#define SW_OFF_PORT     2
#define SW_OFF_PIN      1
#define SW_OFF_MODE     (GPIO_PULLUP | GPIO_INVERT)

// Net VOLUME       - Volume control. Mode: ADC
#define VOLUME_PORT     2
#define VOLUME_PIN      2
#define VOLUME_MODE     GPIO_ANALOG
#define VOLUME_ADC      10

// Net POT_EN       - Power to volume control
#define POT_EN_PORT     2
#define POT_EN_PIN      3
#define POT_EN_MODE     GPIO_OUTPUT

// Net DRIVE        - Output FET drive. Mode: TC5
#define DRIVE_PORT      2
#define DRIVE_PIN       4
#define DRIVE_MODE      GPIO_OUTPUT
#define DRIVE_TC5       0   // OC5A

// Net ISENSE_PASS  - Layout shortcut passthrough for ISENSE
#define ISENSE_PASS_PORT    2
#define ISENSE_PASS_PIN     5
#define ISENSE_PASS_MODE    GPIO_ANALOG

// Net RX           - Debug UART RX. Mode: USART
#define RX_PORT         2
#define RX_PIN          6
#define RX_MODE         GPIO_PULLUP

// Net TX           - Debug UART TX. Mode: USART
#define TX_PORT         2
#define TX_PIN          7
#define TX_MODE         (GPIO_OUTPUT | GPIO_DEF_HI)

/******************************************************************************
 * GPIO WHOLE_PORT DEFINITIONS
 */

#define PORTA_UNUSED    0x01

#define PORTC_UNUSED    0x00
#define PORTC_REMAP     (PORT_TC4C_bm)

#define PORTD_UNUSED    0x00
#define PORTD_REMAP     (PORT_USART0_bm)

#define PORTR_UNUSED    0x03

#endif // HWDEFS_H
