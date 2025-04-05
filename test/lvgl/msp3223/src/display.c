#include "display.h"

bool display_init(struct MSP3223 *display)
{
    if (!ili9341_init(display))
        return false;
    
    if (!ft6336_init(display))
        return false;

    return true;
}
