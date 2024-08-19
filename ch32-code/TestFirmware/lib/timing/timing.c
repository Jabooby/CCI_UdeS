/**
 * @file Timing.cpp
 * @author Renaud Gagnon
 * @brief 
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "timing.h"
uint64_t _millis = 0;

void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
//Timing timing;

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Timer2 initialisation with a 1ms period
 *
 * @return  none
 */
void TIMER2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    // Enable TIM2 clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // Configure the timer to overflow every 1ms
    TIM_TimeBaseStructure.TIM_Period = 999;  // 1ms period (assuming 1 MHz timer clock)
    TIM_TimeBaseStructure.TIM_Prescaler = 71; // Prescaler to slow down the timer clock to 1 MHz (assuming 72 MHz system clock)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    // Enable the TIM2 global Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
}

/*********************************************************************
 * @fn      TIM2_IRQHandler
 *
 * @brief   TIM2 IRQ handler
 *
 * @return  none
 */
void TIM2_IRQHandler(void)
{
    _millis += 1; // Increment by 1ms (1000 microseconds)
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

unsigned long micros()
{
    // Disable interrupts to avoid inconsistencies while reading _micros and timer counter
    __disable_irq();
    unsigned long micros = _millis*1000;
    uint16_t timerCounter = TIM_GetCounter(TIM2);
    __enable_irq();

    // Add the current timer value in microseconds
    micros += timerCounter;

    return micros;
}

u32 millis()
{
    u32 localmillis = 0;
    // Disable interrupts to avoid inconsistencies while reading _micros and timer counter
    __disable_irq();
    localmillis = _millis;
    __enable_irq();


    return millis;
}




