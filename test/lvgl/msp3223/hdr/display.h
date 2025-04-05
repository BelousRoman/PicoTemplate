#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "hardware/spi.h"
#include "hardware/i2c.h"

#include "msp3223_utils.h"
#include "ili9341.h"
#include "ft6336u.h"

// #include "sd.h"

// int msp3223_orientation;
// uint16_t msp3223_width;
// uint16_t msp3223_height;

bool display_init(struct MSP3223 *display);

#endif /* _DISPLAY_H */