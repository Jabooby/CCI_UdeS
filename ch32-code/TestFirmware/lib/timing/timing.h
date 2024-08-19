/**
 * @file Timing.h
 * @author Renaud Gagnon
 * @brief 
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef TIMING_H
#define TIMING_H
#include "debug.h"
#include "string.h"

//extern void TIM2_Init( void );
unsigned long micros();
u32 millis();
void TIMER2_Init(void);

#endif // TIMING_H