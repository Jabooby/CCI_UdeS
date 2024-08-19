/**
 * @file LED_Interface.h
 * @author Renaud Gagnon
 * @brief This file manages the low level interfacing 
 *          of the LEDs on the PCluster project.
 * @version 0.1
 * @date 2023-05-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#ifndef LED_INTERFACE_H
#define LED_INTERFACE_H
#include "ch32v20x_conf.h"
#define DEFAULT_LED_OFF 0x000000
#define DEFAULT_LED_ON 0x0F0F00


extern int GYORColors[11];


void LED_Interface_Init();
void LED_Interface_Set(u8 Channel,u8 Led,u32 Value);
void LED_Interface_Refresh ();
void setled24bVals (u8 nrLed, u32 value);
#endif