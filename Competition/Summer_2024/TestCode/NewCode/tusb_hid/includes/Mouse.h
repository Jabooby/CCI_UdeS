#ifndef MOUSE_H
#define MOUSE_H

#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "class/hid/hid_device.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_system.h"

void MouseControl();
void MouseDelta(int8_t *delta_x_ret, int8_t *delta_y_ret);
void MouseClick(int8_t *Click);
void SetupGPIOMouse();


#endif