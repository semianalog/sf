#ifndef GPIO_H
#define GPIO_H

#include <inttypes.h>
#include <avr/io.h>
#include "hwdefs.h"

/**
 * Initialize all pins. Assumes registers at reset value, so behavior is only
 * defined immediately after reset.
 */
void init_pins(void);

/**
 * Write a value to a pin given the pin's net name and the value. With
 * optimizations `-O2 -flto` enabled, this should compile to a single
 * `sbi` or `cbi` instruction for constant arguments.
 */
#define WRITE_PIN(net, val) write_pin(net ## _PORT, net ## _PIN, val)

/**
 * Read a value from a pin given the pin's net name.
 */
#define READ_PIN(net) read_pin(net ## _PORT, net ## _PIN)

/**
 * Return the VPORT for a given pin given the pin's net name.
 */
#define VPORT_FOR_PIN(net) vport_for_index(net ## _PORT)

/**
 * Return the PORT for a given pin given the pin's net name.
 */
#define PORT_FOR_PIN(net) port_for_index(net ## _PORT)

/**
 * Write a value to a pin given the pin's port number, pin number, and the
 * value. You don't want this, use WRITE_PIN.
 */
void write_pin(uint8_t port, uint8_t pin, bool val);

/**
 * Read a value from a pin given the pin's port number and pin number. You
 * don't want this, use READ_PIN.
 */
bool read_pin(uint8_t port, uint8_t pin);

/**
 * Return the VPORT for a given pin from the port number. You don't want this,
 * use VPORT_FOR_PIN.
 */
VPORT_t *vport_for_index(uint8_t port);

/**
 * Return the PORT for a given pin from the port number. You don't want this,
 * use PORT_FOR_PIN.
 */
PORT_t *port_for_index(uint8_t port);

/* Generate the constants holding port and pin numbers. */
#define X(net, port, pin, dir, defval, pinctrl) \
    static const uint8_t net ## _PORT = port;   \
    static const uint8_t net ## _PIN = pin;
PIN_TABLE ;
#undef X

#endif // GPIO_H
