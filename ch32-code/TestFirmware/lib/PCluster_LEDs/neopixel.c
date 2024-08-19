/**
 * @file neopixel.c
 * @author 
 * @brief 
 * @version 0.1
 * @date 2023-07-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ch32v20x.h"
#include "ch32v20x_spi.h"
#include "ch32v20x_gpio.h"
#include "neopixel.h"
#include <debug.h>
#include <stdint.h>
#include "../../include/main.h"

static uint16_t WS2812dmabuff[DMA_BUFFER_LEN];
static volatile int WS2812LEDs;
static volatile int WS2812LEDPlace;
static volatile int WS2812BLEDInUse;
// This is the code that updates a portion of the WS2812dmabuff with new data.
// This effectively creates the bitstream that outputs to the LEDs.
static void WS2812FillBuffSec( uint16_t * ptr, int numhalfwords, int tce )
{
	const static uint16_t bitquartets[16] = {
		0b1000100010001000, 0b1000100010001110, 0b1000100011101000, 0b1000100011101110,
		0b1000111010001000, 0b1000111010001110, 0b1000111011101000, 0b1000111011101110,
		0b1110100010001000, 0b1110100010001110, 0b1110100011101000, 0b1110100011101110,
		0b1110111010001000, 0b1110111010001110, 0b1110111011101000, 0b1110111011101110, };

	int i;
	uint16_t * end = ptr + numhalfwords;
	int ledcount = WS2812LEDs;
	int place = WS2812LEDPlace;

	while( place < 0 && ptr != end )
	{
		(*ptr++) = 0;
		(*ptr++) = 0;
		(*ptr++) = 0;
		(*ptr++) = 0;
		(*ptr++) = 0;
		(*ptr++) = 0;
		place++;
	}

	while( ptr != end )
	{
		if( place >= ledcount )
		{
			// Optionally, leave line high.
			while( ptr != end )
				(*ptr++) = 0;//0xffff;

			// Only safe to do this when we're on the second leg.
			if( tce )
			{
				if( place == ledcount )
				{
					// Take the DMA out of circular mode and let it expire.
					DMA1_Channel3->CFGR &= ~DMA_Mode_Circular;
					WS2812BLEDInUse = 0;
				}
				place++;
			}

			break;
		}

		// Use a LUT to figure out how we should set the SPI line.
		uint32_t ledval24bit = WS2812BLEDCallback( place++ );

#ifdef WSRBG
		ptr[0] = bitquartets[(ledval24bit>>12)&0xf];
		ptr[1] = bitquartets[(ledval24bit>>8)&0xf];
		ptr[2] = bitquartets[(ledval24bit>>20)&0xf];
		ptr[3] = bitquartets[(ledval24bit>>16)&0xf];
		ptr[4] = bitquartets[(ledval24bit>>4)&0xf];
		ptr[5] = bitquartets[(ledval24bit>>0)&0xf];
#else
		ptr[0] = bitquartets[(ledval24bit>>20)&0xf];
		ptr[1] = bitquartets[(ledval24bit>>16)&0xf];
		ptr[2] = bitquartets[(ledval24bit>>12)&0xf];
		ptr[3] = bitquartets[(ledval24bit>>8)&0xf];
		ptr[4] = bitquartets[(ledval24bit>>4)&0xf];
		ptr[5] = bitquartets[(ledval24bit>>0)&0xf];
#endif
		ptr += 6;
		i += 6;
	}
	WS2812LEDPlace = place;
}

void DMA1_Channel3_IRQHandler( void ) __attribute__((interrupt));
void DMA1_Channel3_IRQHandler( void ) 
{
	//GPIOD->BSHR = 1;	 // Turn on GPIOD0 for profiling

	// Backup flags.
	volatile int intfr = DMA1->INTFR;
	do
	{
		// Clear all possible flags.
		DMA1->INTFCR = DMA1_IT_GL3;

		if( intfr & DMA1_IT_TC3 )
		{
			// Halfwaay (Fill in first part)
			WS2812FillBuffSec( WS2812dmabuff, DMA_BUFFER_LEN / 2, 1 );
		}
		if( intfr & DMA1_IT_HT3 )
		{
			// Complete (Fill in second part)
			WS2812FillBuffSec( WS2812dmabuff + DMA_BUFFER_LEN / 2, DMA_BUFFER_LEN / 2, 0 );
		}
		intfr = DMA1->INTFR;
	} while( intfr );

	//GPIOD->BSHR = 1<<16; // Turn off GPIOD0 for profiling
}

void WS2812BDMAStart( int leds )
{
	if (DMA1_Channel3->CNTR > 0 || DMA1_Channel3->CFGR & DMA_Mode_Circular)
		return;
	// Enter critical section.
	__disable_irq();
	WS2812BLEDInUse = 1;
	DMA1_Channel3->CFGR &= ~DMA_Mode_Circular;
	DMA1_Channel3->CNTR  = 0;
	DMA1_Channel3->MADDR = (uint32_t)WS2812dmabuff;
	WS2812LEDs = leds;
	WS2812LEDPlace = -WS2812B_RESET_PERIOD;
	__enable_irq();

	WS2812FillBuffSec( WS2812dmabuff, DMA_BUFFER_LEN, 0 );

	DMA1_Channel3->CNTR = DMA_BUFFER_LEN; // Number of unique uint16_t entries.
	DMA1_Channel3->CFGR |= DMA_Mode_Circular;
}

void WS2812BDMAInit()
{
	/*
	USART_Printf_Init(115200);
	printf("Initialising NEOPIXEL\r\n");
	GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef  SPI_InitStructure = {0};
	*/
	///* //OLD
	// Enable DMA + Peripherals
	RCC->AHBPCENR |= RCC_AHBPeriph_DMA1;
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1;

	// MOSI, Configure GPIO Pin
	GPIOA->CFGLR &= ~(0xf<<(4*7));
	GPIOA->CFGLR |= (GPIO_Speed_50MHz | GPIO_CNF_OUT_PP_AF)<<(4*7); 
	//*/



	// Configure SPI 
	///*//Old
	SPI1->CTLR1 = 
		SPI_NSS_Soft | SPI_CPHA_1Edge | SPI_CPOL_Low | SPI_DataSize_16b |
		SPI_Mode_Master | SPI_Direction_1Line_Tx |
		SPI_BaudRatePrescaler_16; // Divisior = 16 (48/16 = 3MHz)

	SPI1->CTLR2 = SPI_CTLR2_TXDMAEN;
	SPI1->HSCR = 1;

	SPI1->CTLR1 |= CTLR1_SPE_Set;

	SPI1->DATAR = 0; // Set SPI line Low.
	//*/

	 //NEW
	/*
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
	SPI_Cmd(SPI1, ENABLE);
	 */

	// Configure DMA
	 //OLD
	//DMA1_Channel3 is for SPI1TX
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA1_Channel3->PADDR = (uint32_t)&SPI1->DATAR;
	DMA1_Channel3->MADDR = (uint32_t)WS2812dmabuff;
	DMA1_Channel3->CNTR  = 0;// sizeof( bufferset )/2; // Number of unique copies.  (Don't start, yet!)
	DMA1_Channel3->CFGR  =
		DMA_M2M_Disable |		 
		DMA_Priority_VeryHigh |
		DMA_MemoryDataSize_HalfWord |
		DMA_PeripheralDataSize_HalfWord |
		DMA_MemoryInc_Enable |
		DMA_Mode_Normal | // OR DMA_Mode_Circular or DMA_Mode_Normal
		DMA_DIR_PeripheralDST |
		DMA_IT_TC | DMA_IT_HT; // Transmission Complete + Half Empty Interrupts. 

//	NVIC_SetPriority( DMA1_Channel3_IRQn, 0<<4 ); //We don't need to tweak priority.
	NVIC_EnableIRQ( DMA1_Channel3_IRQn );
	DMA1_Channel3->CFGR |= DMA_CFGR1_EN;
	

	/*//  NEW
	DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel3);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DATAR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)WS2812dmabuff;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    DMA_ClearFlag(DMA1_FLAG_TC3);

    DMA_Cmd(DMA1_Channel3, ENABLE);
	 */
}