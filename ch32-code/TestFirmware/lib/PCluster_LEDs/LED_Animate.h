/**
 * @file LED_Animate.h
 * @author Renaud Gagnon
 * @brief This file enables animations and display 
 *          management of the LEDs of the PCluster project
 * @version 0.1
 * @date 2023-05-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef LED_ANIMATE_H
#include "ch32v20x_conf.h"
#include "LED_Interface.h"
#define LEDMODE_OFF 0
#define LEDMODE_SOLID 1
#define LEDMODE_SOLIDpp 10
#define LEDMODE_GRADUAL 2
#define LEDMODE_FOLLOW 11
#define LEDMODE_RAINBOW 3
#define LEDMODE_CUSTOM 12

void LEDAnimate_init();
void LEDAnimate_update(u8 LEDMode, u8 LEDBrightness, u8 *values);
#endif