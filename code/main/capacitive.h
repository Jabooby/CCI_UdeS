#pragma once

#include "driver/touch_pad.h"

#define TOUCH_BUTTON_NUM 8
#define TOUCH_CHANGE_CONFIG 0

static const touch_pad_t button[TOUCH_BUTTON_NUM] = {
    TOUCH_PAD_NUM1,
    TOUCH_PAD_NUM2,
    TOUCH_PAD_NUM3,
    TOUCH_PAD_NUM4,
    TOUCH_PAD_NUM5,
    TOUCH_PAD_NUM6,
    TOUCH_PAD_NUM7,
    TOUCH_PAD_NUM8};

static void capacitive_read_task();