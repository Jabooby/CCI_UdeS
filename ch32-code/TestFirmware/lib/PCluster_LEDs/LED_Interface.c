/**
 * @file LED_Interface.c
 * @author Renaud Gagnon
 * @brief This file manages the low level interfacing 
 *          of the LEDs on the PCluster project.
 * @version 0.1
 * @date 2023-06-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "LED_Interface.h"
#include "neopixel.h"


// WS2812 defines
#define WS2812DMA_IMPLEMENTATION
#define WSRBG      // For WS2816C's.
//#ifdef LEDS_CONFIG_ALL
//#define NR_LEDS 50 // Number of leds on the led assembly
//#else
//#define NR_LEDS 62 // Number of leds on the led assembly
//#endif

#define NR_LEDS 62 // Number of leds on the led assembly


// int led24bVals[NR_LEDS] = {0};

int led24bVals[NR_LEDS] = {0};

int i;
int *ledsPr[4][13];

int GYORColors[11] = {
    0x00FF00, // Pure green
    0x55FF00, // Green with a hint of yellow
    0xAAFF00, // Yellowish green
    0xFFFF00, // Pure yellow
    0xFFD400, // Yellow with a hint of red
    0xFFAA00, // Orange-yellow
    0xFF7F00, // Deep orange
    0xFF5500, // Orange-red
    0xFF2A00, // Reddish-orange
    0x7F0000, // Pure red
    0xFF0000  // Dark red
};

uint32_t WS2812BLEDCallback(int ledno)
{
    /*
    uint8_t index = (phases[ledno])>>8;
    uint8_t rsbase = sintable[index];
    uint8_t rs = rsbase>>3;
    uint32_t fire = ((huetable[(rs+190)&0xff]>>1)<<16) | (huetable[(rs+30)&0xff]) | ((huetable[(rs+0)]>>1)<<8);
    uint32_t ice  = 0x7f0000 | ((rsbase>>1)<<8) | ((rsbase>>1));

    // Because this chip doesn't natively support multiplies, we are going to avoid tweening of 1..254.
    return TweenHexColors( fire, ice, ((tween + ledno)>0)?255:0 ); // Where "tween" is a value from 0 ... 255
    */
    //return 0x00FF00;
    return led24bVals[ledno]; // 0xRRGGBB
}

void LED_Interface_Init()
{
    ledsPr[0][0] = &led24bVals[0];
    ledsPr[0][1] = &led24bVals[1];
    ledsPr[0][2] = &led24bVals[2];
    ledsPr[0][3] = &led24bVals[3];
    ledsPr[0][4] = &led24bVals[4];
    ledsPr[0][5] = &led24bVals[5];
    ledsPr[0][6] = &led24bVals[6];
    ledsPr[0][7] = &led24bVals[7];
    ledsPr[0][8] = &led24bVals[8];
    ledsPr[0][9] = &led24bVals[9];
    ledsPr[0][10] = &led24bVals[10];
    ledsPr[0][11] = &led24bVals[11];
    ledsPr[0][12] = &led24bVals[12];

    ledsPr[1][0] = &led24bVals[13];
    ledsPr[1][1] = &led24bVals[14];
    ledsPr[1][2] = &led24bVals[15];
    ledsPr[1][3] = &led24bVals[16];
    ledsPr[1][4] = &led24bVals[17];
    ledsPr[1][5] = &led24bVals[18];
    ledsPr[1][6] = &led24bVals[19];
    ledsPr[1][7] = &led24bVals[20];
    ledsPr[1][8] = &led24bVals[21];
    ledsPr[1][9] = &led24bVals[22];
    ledsPr[1][10] = &led24bVals[23];
    ledsPr[1][11] = &led24bVals[24];
    ledsPr[1][12] = &led24bVals[25];

    ledsPr[2][0] = &led24bVals[26];
    ledsPr[2][1] = &led24bVals[27];
    ledsPr[2][2] = &led24bVals[28];
    ledsPr[2][3] = &led24bVals[29];
    ledsPr[2][4] = &led24bVals[30];
    ledsPr[2][5] = &led24bVals[31];
    ledsPr[2][6] = &led24bVals[32];
    ledsPr[2][7] = &led24bVals[33];
    ledsPr[2][8] = &led24bVals[34];
    ledsPr[2][9] = &led24bVals[35];
    ledsPr[2][10] = &led24bVals[36];
    ledsPr[2][11] = &led24bVals[37];
    ledsPr[2][12] = &led24bVals[38];

    ledsPr[3][0] = &led24bVals[39];
    ledsPr[3][1] = &led24bVals[40];
    ledsPr[3][2] = &led24bVals[41];
    ledsPr[3][3] = &led24bVals[42];
    ledsPr[3][4] = &led24bVals[43];
    ledsPr[3][5] = &led24bVals[44];
    ledsPr[3][6] = &led24bVals[45];
    ledsPr[3][7] = &led24bVals[46];
    ledsPr[3][8] = &led24bVals[47];
    ledsPr[3][9] = &led24bVals[48];
    ledsPr[3][10] = &led24bVals[49];
    ledsPr[3][11] = &led24bVals[50];
    ledsPr[3][12] = &led24bVals[51];

    
    WS2812BDMAInit();
    WS2812BDMAStart(NR_LEDS);
}

/**
 * @brief Function that sends the buffered values to the LEDs
 * Should be called every time that LED_Interface_Set
 * is used in order for the changes to take effect.
 *
 */
void LED_Interface_Refresh()
{
    WS2812BDMAStart(NR_LEDS);
}

/**
 * @brief This function is used to set the value of the LEDs of the device
 *
 * @param Channel 0 = All channels, 1 to 4 = specific channel
 * @param Led 0 = All leds, 1 to 11 = specific LED (12 = center led)
 * @param Value 24bit color value that is to be sent to the selected LED(s)
 */
void LED_Interface_Set(u8 channel, u8 led, u32 value)
{
    if (channel > 4 )return;
    if (led > 12)return;
    if (value > 0xFFFFFF) return;

    if (channel == 0) // Set all channels
    {
        for (int i = 0; i <= 3; i++) // For All channels
        {
            if (led == 0) // Set All leds of all channels
            {
                for (int i2 = 0; i2 <= 10; i2++)    // For all leds
                {
                    *ledsPr[i][i2] = value;
                }
            }
            else    // Set specific led of all channels
                *ledsPr[i][led-1] = value;
            if (led == 12)  
                *ledsPr[i][12] = value; // Set the other center led
        }
    }
    else if (led == 0) // Set all leds of a specific channel
    {
        for (int i = 0; i <= 10; i++)   // For all leds
        {
            *ledsPr[channel][i] = value;
        }
    }
    else if (channel <= 4) *ledsPr[channel-1][led-1] = value;   // Set specific led of a specific channel

    //else if (channel == 5)
    //{
    //    for (int i = 0; i <= 3; i++)
    //    {
    //        *ledsPr[i][led-1] = value;
    //    }
    //}
}

void setled24bVals (u8 nrLed, u32 value)
{
    led24bVals[nrLed] = value;
}

