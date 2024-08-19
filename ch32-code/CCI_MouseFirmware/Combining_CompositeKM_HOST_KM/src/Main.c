/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/08/08
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 * @Note
 * Composite Keyboard and Mouse Example:
 * This example uses PA4-PA7 and PB12-PB15 to simulate mouse movement and keyboard
 * key pressing respectively, active low. At the same time, it also uses USART2
 * to receive data sent from the host to simulate keyboard key pressing and releasing.
 *
 */

#include "debug.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb_prop.h"
#include "usbd_composite_km.h"
#include "usb_host_config.h"
#include "wiring.h"
#include "joystick.h"

#include "ch32v20x_adc.h"
#include "ch32v20x_gpio.h"
#include "ch32v20x_rcc.h"


void vibrate(uint8_t value)
{
    digitalWrite(pin_A8, value);
}

void ADC_Config(void)
{
    // Enable the clock for ADC1 and GPIOA
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

    // Configure PA7 as analog input
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure ADC1
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // Configure the ADC to read from channel 7 (PA7)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);

    // Enable ADC1 and calibrate
    ADC_Cmd(ADC1, ENABLE);

    // Start the calibration process
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1))
        ;
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1))
        ;
}

uint16_t ADC_Read(void)
{
    // Start the conversion
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    // Wait until conversion is complete
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
        ;

    // Return the result of the conversion
    return ADC_GetConversionValue(ADC1);
}

/* Global define */

/* Global Variable */
struct Joystick joystick;

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{

    portEnable(port_A);
    portEnable(port_B);
    // pinMode(pin_B7, pinMode_I_analog);    // U/D
    // pinMode(pin_B0, pinMode_I_analog);    // L/R
    pinMode(pin_B10, pinMode_I_floating); // BTN
    pinMode(pin_A8, pinMode_O_pushPull);  // motor
    // ADC_Config();
    uint8_t motState = 1;
    digitalWrite(pin_A8, motState);

    // joystick.lr = 0;
    // joystick.ud = 0;
    joystick.btn = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("USBD Keyboard&Mouse Demo\r\n");

    // Usb Device inits
    /* Initialize USART2 for receiving the specified keyboard data */
    USART2_Init(115200);
    printf("USART2 Init OK!\r\n");

    /* Initialize GPIO for keyboard scan */
    // KB_Scan_Init( );
    // KB_Sleep_Wakeup_Cfg( );
    printf("KB Scan Init OK!\r\n");

    /* Initialize GPIO for mouse scan */
    // MS_Scan_Init( );
    // MS_Sleep_Wakeup_Cfg( );
    printf("MS Scan Init OK!\r\n");

    /* Initialize timer for Keyboard and mouse scan timing */
    TIM3_Init(1, 7199);
    printf("TIM3 Init OK!\r\n");

    Set_USBConfig();
    USB_Init();
    USB_Interrupts_Config();

    // Usb Host inits
    /* Initialize TIM3 */
    TIM3_Init(9, SystemCoreClock / 10000 - 1);
    DUG_PRINTF("TIM3 Init OK!\r\n");

    /* Initialize USBFS host */
#if DEF_USBFS_PORT_EN
    DUG_PRINTF("USBFS Host Init\r\n");
    USBFS_RCC_Init();
    USBFS_Host_Init(ENABLE);
    memset(&RootHubDev.bStatus, 0, sizeof(ROOT_HUB_DEVICE));
    memset(&HostCtl[DEF_USBFS_PORT_INDEX * DEF_ONE_USB_SUP_DEV_TOTAL].InterfaceNum, 0, DEF_ONE_USB_SUP_DEV_TOTAL * sizeof(HOST_CTL));
#endif

    motState = 0;
    digitalWrite(pin_A8, motState);

    while (1)
    {
        //joystick.ud = ADC_Read();
        joystick.btn = !digitalRead(pin_B10);
        // digitalWrite(pin_A8, joystick.btn);
        if (joystick.btn)
            vibrate(1);

        // joystick.lr = analogRead(pin_B0);
        // joystick.ud = analogRead(pin_A7);

        if ((bDeviceState == CONFIGURED)) //&& (USBFSH_CheckRootHubPortStatus(RootHubDev.bStatus) == ROOT_DEV_CONNECTED))
        {

            /* Handle keyboard scan data */
            //KB_Scan_Handle();

            /* Handle keyboard lighting */
            //KB_LED_Handle();

            /* Handle mouse scan data */
            MS_Scan_Handle();

            /* Handle USART2 receiving data */
            USART2_Receive_Handle();
        }
        USBH_MainDeal();
    }
}
