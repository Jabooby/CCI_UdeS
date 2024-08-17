/**
 * This example turns the ESP32 into a Bluetooth LE mouse that scrolls down every 2 seconds.
 */
#include <Adafruit_ILI9341.h>
#include <MPU9250_WE.h>
#include <BleMouse.h>
#include <Arduino.h>

BleMouse bleMouse;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleMouse.begin();
}

void loop() {
  if(bleMouse.isConnected()) {
    Serial.println("Scroll Down");
    bleMouse.move(0,0,-1);
  }
  delay(2000);
}