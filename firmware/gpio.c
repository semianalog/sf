#include "hwdefs.h"
#include "gpio.h"

static inline void init_pins_out(void);
static inline void init_pins_dir(void);
static inline void init_pins_ctrl(void);
static inline PORT_t *port_for_index(uint8_t port);
static inline VPORT_t *vport_for_index(uint8_t port);
static inline uint8_t volatile *pinctrl_for_pin(PORT_t *port, uint8_t pin);

void init_pins(void)
{
    PORTCFG.SRLCTRL =
        PORTCFG_SRLENRA_bm |
        PORTCFG_SRLENRC_bm |
        PORTCFG_SRLENRD_bm |
        PORTCFG_SRLENRR_bm;

    init_pins_out();
    init_pins_dir();
    init_pins_ctrl();

    PORTC.REMAP = PORTC_REMAP_VAL;
    PORTD.REMAP = PORTD_REMAP_VAL;
}

void write_pin(uint8_t port, uint8_t pin, bool val)
{
    if (val) {
        vport_for_index(port)->OUT |= (1 << pin);
    } else {
        vport_for_index(port)->OUT &= (1 << pin);
    }
}

bool read_pin(uint8_t port, uint8_t pin)
{
    return vport_for_index(port)->IN & (1 << pin);
}

static inline void init_pins_out(void)
{
    uint8_t outs[4] = {0};

#define X(net, port, pin, dir, defval, pinctrl) \
    if (defval) outs[port] |= (1 << pin);
    PIN_TABLE ;
#undef X

    if (outs[0]) VPORT0.OUT = outs[0];
    if (outs[1]) VPORT1.OUT = outs[1];
    if (outs[2]) VPORT2.OUT = outs[2];
    if (outs[3]) VPORT3.OUT = outs[3];
}

static inline void init_pins_dir(void)
{
    uint8_t dirs[4] = {0};

#define X(net, port, pin, dir, defval, pinctrl) \
    if (dir) dirs[port] |= (1 << pin);
    PIN_TABLE ;
#undef X

    if (dirs[0]) VPORT0.OUT = dirs[0];
    if (dirs[1]) VPORT1.OUT = dirs[1];
    if (dirs[2]) VPORT2.OUT = dirs[2];
    if (dirs[3]) VPORT3.OUT = dirs[3];
}

static inline void init_pins_ctrl(void)
{
#define X(net, port, pin, dir, defval, pinctrl) \
    if (pinctrl) { \
        *pinctrl_for_pin(port_for_index(port), pin) = pinctrl; \
    }
    PIN_TABLE ;
#undef X
}

static inline PORT_t *port_for_index(uint8_t port)
{
    switch (port) {
    case 0: return &PORTA;
    case 1: return &PORTC;
    case 2: return &PORTD;
    case 3: return &PORTR;
    default: for(;;);
    }
}

static inline VPORT_t *vport_for_index(uint8_t port)
{
    switch (port) {
    case 0: return &VPORT0;
    case 1: return &VPORT1;
    case 2: return &VPORT2;
    case 3: return &VPORT3;
    default: for(;;);
    }
}

static inline uint8_t volatile *pinctrl_for_pin(PORT_t *port, uint8_t pin)
{
    if (pin < 8) {
        return (&port->PIN0CTRL) + pin;
    } else {
        for(;;);
    }
}
