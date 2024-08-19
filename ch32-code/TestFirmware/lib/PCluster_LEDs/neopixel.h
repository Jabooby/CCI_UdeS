/* Single-File-Header for using asynchronous LEDs with the CH32V003 using DMA to the SPI port.
   I may write another version of this to use DMA to timer ports, but, the SPI port can be used
   to generate outputs very efficiently. So, for now, SPI Port.  Additionally, it uses FAR less
   internal bus resources than to do the same thing with timers.
   
   For the CH32V003 this means output will be on PORTC Pin 6

   Copyright 2023 <>< Charles Lohr, under the MIT-x11 or NewBSD License, you choose!

   If you are including this in main, simply 
	#define WS2812DMA_IMPLEMENTATION

   You will need to implement the following two functions, as callbacks from the ISR.
	uint32_t CallbackWS2812BLED( int ledno );

   You willalso need to call
	InitWS2812DMA();

   Then, whenyou want to update the LEDs, call:
	WS2812BStart( int num_leds );
*/

#ifndef _WS2812_LED_DRIVER_H
#define _WS2812_LED_DRIVER_H

#include <stdint.h>

// Use DMA and SPI to stream out WS2812B LED Data via the MOSI pin.
void WS2812BDMAInit( );
void WS2812BDMAStart( int leds );

// Callbacks that you must implement.
uint32_t WS2812BLEDCallback( int ledno );


// Must be divisble by 4.
#define DMALEDS 108
#define WSRBG
// Note first n LEDs of DMA Buffer are 0's as a "break"
// Need one extra LED at end to leave line high. 
// This must be greater than WS2812B_RESET_PERIOD.
#define WS2812B_RESET_PERIOD 2
#define DMA_BUFFER_LEN (((DMALEDS))*6+1) // The +1 is for the buffered start.

#endif



