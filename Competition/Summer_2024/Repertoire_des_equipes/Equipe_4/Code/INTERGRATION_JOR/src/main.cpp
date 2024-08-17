#include <Arduino.h>
/**
 * TCA9548 I2CScanner.ino -- I2C bus scanner for Arduino
 *
 * Based on https://playground.arduino.cc/Main/I2cScanner/
 *
 */

#include "Wire.h"
#include "SPI.h"
#include <Adafruit_ILI9341.h>
#include <dragon.h>
#define LCD_PIN_CS 14
#define LCD_PIN_RESET 13
#define LCD_PIN_DC 12
#define LCD_PIN_SDI 11
#define LCD_PIN_SCK 10
#define LCD_PIN_SDO 9


#include <MPU9250_WE.h>
#define BNO_SDA 2
#define BNO_SCL 1
#define MPU_ADDR 0x68



Adafruit_ILI9341 tft = Adafruit_ILI9341(LCD_PIN_CS, LCD_PIN_DC, LCD_PIN_SDI, LCD_PIN_SCK, LCD_PIN_RESET, LCD_PIN_SDO);
MPU9250_WE myMPU9250 = MPU9250_WE(MPU_ADDR);

void setup() {
  tft.begin();

  Serial.begin(115200);
  Wire.begin(BNO_SDA, BNO_SCL);
  if(!myMPU9250.init()){
    Serial.println("MPU9250 does not respond");
  }
  else{
    Serial.println("MPU9250 is connected");
  }

  Serial.println("Position you MPU9250 flat and don't move it - calibrating...");
  delay(1000);
  myMPU9250.autoOffsets();
  Serial.println("Done!");

  myMPU9250.setSampleRateDivider(5);

  myMPU9250.setAccRange(MPU9250_ACC_RANGE_2G);
  myMPU9250.enableAccDLPF(true);
  myMPU9250.setAccDLPF(MPU9250_DLPF_6);


}

void printBno() {
  xyzFloat accRaw = myMPU9250.getAccRawValues();
  xyzFloat accCorrRaw = myMPU9250.getCorrectedAccRawValues();
  xyzFloat gValue = myMPU9250.getGValues();
  float resultantG = myMPU9250.getResultantG(gValue);
  
  Serial.println("Raw acceleration values (x,y,z):");
  Serial.print(accRaw.x);
  Serial.print("   ");
  Serial.print(accRaw.y);
  Serial.print("   ");
  Serial.println(accRaw.z);

  Serial.println("Corrected ('calibrated') acceleration values (x,y,z):");
  Serial.print(accCorrRaw.x);
  Serial.print("   ");
  Serial.print(accCorrRaw.y);
  Serial.print("   ");
  Serial.println(accCorrRaw.z);

  Serial.println("g values (x,y,z):");
  Serial.print(gValue.x);
  Serial.print("   ");
  Serial.print(gValue.y);
  Serial.print("   ");
  Serial.println(gValue.z);

  Serial.print("Resultant g: ");
  Serial.println(resultantG); // should always be 1 g if only gravity acts on the sensor.
  Serial.println();
}

void loop(void) {
  printBno();

  for(uint8_t r=0; r<4; r++) {
    tft.setRotation(r);
    tft.fillScreen(ILI9341_BLACK);
    for(uint8_t j=0; j<20; j++) {
      tft.drawRGBBitmap(
        random(-DRAGON_WIDTH , tft.width()),
        random(-DRAGON_HEIGHT, tft.height()),
#if defined(__AVR__) || defined(ESP8266)
        dragonBitmap,
#else
        // Some non-AVR MCU's have a "flat" memory model and don't
        // distinguish between flash and RAM addresses.  In this case,
        // the RAM-resident-optimized drawRGBBitmap in the ILI9341
        // library can be invoked by forcibly type-converting the
        // PROGMEM bitmap pointer to a non-const uint16_t *.
        (uint16_t *)dragonBitmap,
#endif
        DRAGON_WIDTH, DRAGON_HEIGHT);
      delay(1); // Allow ESP8266 to handle watchdog & WiFi stuff
    }
    delay(3000);
  }
}

