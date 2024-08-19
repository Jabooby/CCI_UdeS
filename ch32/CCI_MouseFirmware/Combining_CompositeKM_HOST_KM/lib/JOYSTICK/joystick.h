#pragma once
#include <stdint.h>

struct Joystick
{
    uint16_t lr;
    uint16_t ud;
    bool btn;
};

extern struct Joystick joystick;