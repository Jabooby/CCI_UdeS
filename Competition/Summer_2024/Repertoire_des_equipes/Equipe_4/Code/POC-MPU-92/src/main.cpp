#include <Arduino.h>
/**
 * TCA9548 I2CScanner.ino -- I2C bus scanner for Arduino
 *
 * Based on https://playground.arduino.cc/Main/I2cScanner/
 *
 */

#include "Wire.h"

int number = 0;

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("HELLO_WORLD!");
  Serial.println("We are simply flashing a LED on pin 13 every 500ms");
  pinMode(0xF, OUTPUT);
}

void loop() 
{
  Serial.print("Ping ");
  Serial.println(number++);
  digitalWrite(0x0F, digitalRead(0x0F) ^ 1);
  delay(500);
}