/**
 * @file mcu_reset.h
 * @author Renaud Gagnon
 * @brief This file provides a function that resets the CH32V2xx and CH32V3xx MCUs
 * @version 0.1
 * @date 2023-07-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "main.h"
#define RESET_REGISTER_ADDRESS 0xE000E048 //PFIC_CFGR register
#define RESET_MASK 0xBEEF0080 //KEYCODE: KEY3 = 0xBEEF and 0x80 for reset bit


void ResetMCU (void);

void ResetMCU (void)
{
    volatile uint32_t *resetRegister = (volatile uint32_t *)RESET_REGISTER_ADDRESS;
    *resetRegister |= RESET_MASK;
}

void bootResetMCU (void);

void bootResetMCU (void)
{
    //Bootloader reset pin
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOB, GPIO_Pin_1, 1);

    Delay_Ms(20);

    //Reset MCU
    volatile uint32_t *resetRegister = (volatile uint32_t *)RESET_REGISTER_ADDRESS;
    *resetRegister |= RESET_MASK;
}

void GetUNIQUE_ID(uint32_t *id);


void GetUNIQUE_ID(uint32_t *id)
{
    id[0] = *(uint32_t *)0x1FFFF7E8;
    id[1] = *(uint32_t *)0x1FFFF7EC;
    id[2] = *(uint32_t *)0x1FFFF7F0;
}