/**
 * @file LED_Animate.c
 * @author Renaud Gagnon
 * @brief This file enables animations and display 
 *          management of the LEDs of the PCluster project
 * @version 0.1
 * @date 2023-05-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "LED_Animate.h"
#include <stdlib.h>

#define SOLID_COLOR 0x0F0F0F
//Private function defines
void ledUpdateOFF ();       //All leds to OFF
void ledUpdateSolid (u8 brightness);     //All leds to a single predefined color
void ledUpdateSolidpp (u8 *values, u8 brightness);   //ALL leds of a same display to a single color that depends on the displayed value
void ledUpdateGradual (u8 *values, u8 brightness);   //Leds under the value of the needle light up from green to red
void ledUpdateFollow (u8 *values, u8 brightness);    //The led that is under the needle light up from green to red
void ledUpdateRainbow (u8 brightness);   //All leds do a green to red gradient 
void ledUpdateCustom ();


uint32_t generateColor(uint8_t load, uint8_t brightness);

void LEDAnimate_update(u8 LEDMode, u8 LEDBrightness, u8 *values)
{
    switch (LEDMode)
    {
        case LEDMODE_OFF:
        ledUpdateOFF (); 
        break;

        case LEDMODE_SOLID:
        ledUpdateSolid (LEDBrightness);
        break;

        case LEDMODE_SOLIDpp:
        ledUpdateSolidpp (values, LEDBrightness);
        break;

        case LEDMODE_GRADUAL:
        ledUpdateGradual (values, LEDBrightness);
        break;

        case LEDMODE_FOLLOW:
        ledUpdateFollow (values, LEDBrightness);
        break;

        case LEDMODE_RAINBOW:
        ledUpdateRainbow (LEDBrightness);
        break;

        case LEDMODE_CUSTOM:
        ledUpdateCustom ();
        break;

    }
    LED_Interface_Refresh();
}

void ledUpdateOFF ()       //All leds to OFF
{
    LED_Interface_Set(0, 0, 0);
}
void ledUpdateSolid (u8 brightness)     //All leds to a single predefined color
{
    u32 color = SOLID_COLOR;
    color = (((color >> 16) & 0xFF) * brightness / 100) << 16 | (((color >> 8) & 0xFF) * brightness / 100) << 8 | ((color & 0xFF) * brightness / 100);
    LED_Interface_Set(0, 0, color);
}

void ledUpdateSolidpp (u8 *values, u8 brightness)   //ALL leds of a same display to a single color that depends on the displayed value
{
    for (int i = 0 ; i <= 3 ; i++)
    {
        //LED_Interface_Set(i+1, 0, generateColor(values[i], brightness));
    }
}

//Done. Needs testing
void ledUpdateGradual (u8 *values, u8 brightness)   //Leds under the value of the needle light up from green to red
{
    for(int iDisplay = 0 ; iDisplay <= 3 ; iDisplay++)
    {
        for (int iLed = 0; iLed <= 10; iLed++)
        {
            if((iLed*10)+1 < (u8)values[iDisplay])   //Test if LED addressed by iLed below or close to the needle
            {
                if ((iLed * 10) > values[iDisplay]) //Test if addressed LED is above the needle
                {
                    //Apply a faded effect to account for the fact that the needle has not reached the LED yet
                    LED_Interface_Set(iDisplay+1, iLed+1, generateColor(iLed*10, brightness*values[iDisplay]%10));

                }
                else
                {
                    //Set the led's value to the corresponding "rainbow color" and desired brightness
                    LED_Interface_Set(iDisplay+1, iLed+1, generateColor(iLed*10, brightness));
                }
            }
            else 
            {
                LED_Interface_Set(iDisplay+1, iLed+1, 0x000000);
                
            }
            
                
        }
    }
    //LED_Interface_Set(0, 0, 0xFF0000);
}

//Done. Needs testing.
void ledUpdateFollow (u8 *values, u8 brightness)    //The led that is under the needle light up from green to red
{
    for (int iDisplay = 0; iDisplay <= 10; iDisplay++)
    {
        for (int iLed = 0; iLed <= 10; iLed++)
        {
            if (values[iDisplay]%10 == 0)
            {
                LED_Interface_Set(iDisplay+1, iLed+1, generateColor(iLed*10, brightness));
            }
            else if (iLed*10 == ((u8)values[iDisplay]/10) || //Test if LED is directly below the needle
            iLed*10 == ((u8)values[iDisplay]/10)+1) //Test if LED is directly above the needle
            {
                LED_Interface_Set(iDisplay+1, iLed+1, generateColor(iLed*10, brightness*(abs(values[iDisplay] - iLed*10))));
            }
        }
    }
}

void ledUpdateRainbow (u8 brightness)   //All leds do a green to red gradient 
{
    for (int i = 1 ; i<= 11 ; i++)
    {
        LED_Interface_Set(0, i, generateColor((i-1)*10, brightness));
    }    
}
void ledUpdateCustom ()
{

}

//Utilities finctions

//Brightness = 0 to 255
//Load = 0 to 100
uint32_t generateColor(uint8_t load, uint8_t brightness) 
{
    if(brightness>100 || load > 100)return 0;
    // Calculate the individual color components based on the load and brightness
    uint8_t green = (255 * (100 - load) * brightness) / 10000;
    uint8_t red = (255 * load * brightness) / 10000;
    uint8_t blue = 0;

    // Combine the color components into a 24-bit color value (RGB format)
    uint32_t color = (red << 16) | (green << 8) | blue;

    return color;
}

void LEDAnimate_init()
{
    LED_Interface_Init();
    LED_Interface_Set(0, 0, 0x000000);
    LED_Interface_Refresh();
}