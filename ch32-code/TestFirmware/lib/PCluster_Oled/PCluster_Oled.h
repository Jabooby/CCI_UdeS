/*
 * Single-File-Header for using I2C OLED
 * 03-29-2023 E. Brombaugh
 */

#ifndef _OLED_H
#define _OLED_H

#include <stdint.h>
#include <string.h>
#include "i2c_custom.h"
//#include "i2c.h"
#include "font_8x8.h"
#include "../../include/main.h"
// OLED I2C address
#define OLED_ADDR 0x3c
#define PIN_DISP_RST   pin_B9	//Added RG

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2
#define SSD1306_TERMINATE_CMDS 0xFF

/* choose VCC mode */
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2
//#define vccstate SSD1306_EXTERNALVCC
#define vccstate SSD1306_SWITCHCAPVCC

#define OLED_W 128
#define OLED_H 32

#define OLED_PSZ 32		// comfortable I2C packet size for this OLED

/*
 * enum for font size
 */
typedef enum {
    fontsize_8x8 = 1,
    fontsize_16x16 = 2,
    fontsize_32x32 = 4
} font_size_t;

void oled_setbuf(uint8_t color);
void oled_refresh(void);
void oled_drawPixel(uint8_t x, uint8_t y, uint8_t color);
void oled_xorPixel(uint8_t x, uint8_t y);
void oled_drawFastVLine(uint8_t x, uint8_t y, uint8_t h, uint8_t color);
void oled_drawFastHLine(uint8_t x, uint8_t y, uint8_t w, uint8_t color);
int16_t gfx_abs(int16_t x);
void gfx_swap(uint16_t *z0, uint16_t *z1);
void oled_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t color);
void oled_drawCircle(int16_t x, int16_t y, int16_t radius, int8_t color);
void oled_fillCircle(int16_t x, int16_t y, int16_t radius, int8_t color);
void oled_drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
void oled_fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
void oled_xorrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void oled_drawchar(uint8_t x, uint8_t y, uint8_t chr, uint8_t color);
void oled_drawstr(uint8_t x, uint8_t y, char *str, uint8_t color);
void oled_drawchar_sz(uint8_t x, uint8_t y, uint8_t chr, uint8_t color, font_size_t font_size);
void oled_drawstr_sz(uint8_t x, uint8_t y, char *str, uint8_t color, font_size_t font_size);
uint8_t oled_init(void);
void oled_sendInitCommands();
#endif
