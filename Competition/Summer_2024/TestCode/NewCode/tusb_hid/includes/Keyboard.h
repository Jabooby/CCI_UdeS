#ifndef KEYBORAD_H
#define KEYBORAD_H

#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "class/hid/hid_device.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_system.h"


void SetupGPIOKeyboard();
void WriteASCII(uint8_t chr);
void WriteWordsASCII(uint8_t *chars, uint8_t size);
void WriteCharacter(unsigned long chr);
void WriteCombinationOfCharacterASCII(unsigned char chr);
void WriteCombinationOfCharacter(unsigned long chr);
void WriteEndCombination();

#endif