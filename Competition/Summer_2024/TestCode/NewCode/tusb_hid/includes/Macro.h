#ifndef MACRO_H
#define MACRO_H

#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "class/hid/hid_device.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_system.h"
#include "Keyboard.h"

void OpenCMD();
void OpenNotePad();
void WriteThis();
void GetRickRolled();
void MatrixAttack(char *str, uint8_t size);

#endif