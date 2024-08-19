/**
 * @file i2c_custom.h
 * @author Renaud Gagnon
 * @brief This file is a driver for the ch32v003. 
 *          It enables multiplexing on 4 different scl lines but one sda.
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef I2C_CUSTOM_H
#define I2C_CUSTOM_H
#include "../../include/main.h"
#include <wiring.h>

//Pin defines
#define PIN_SDA  pin_B5
#define PIN_SCL1 pin_B8
#define PIN_SCL2 pin_B3
#define PIN_SCL3 pin_B4
#define PIN_SCL4 pin_A15


void i2c_init();
void I2C_SelectChannel (u8 selection);
u8 i2c_send(uint8_t addr, uint8_t *data, uint8_t sz);

#endif
