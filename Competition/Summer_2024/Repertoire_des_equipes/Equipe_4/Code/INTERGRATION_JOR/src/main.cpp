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

#define ENCODER_LEFT_PIN_A 41
#define ENCODER_LEFT_PIN_B 42
#define ENCODER_LEFT_PIN_BUTTON 48

#define ENCODER_RIGHT_PIN_A 39
#define ENCODER_RIGHT_PIN_B 40
#define ENCODER_RIGHT_PIN_BUTTON 47

volatile int encoderPosLeft = 0;  // a counter for the dial
unsigned int lastReportedPosLeft = 1;   // change management
static boolean rotatingLeft=false;      // debounce management

volatile int encoderPosRight = 0;  // a counter for the dial
unsigned int lastReportedPosRight = 1;   // change management
static boolean rotatingRight=false;      // debounce management

// interrupt service routine vars
boolean A_set_left = false;              
boolean B_set_left = false;

boolean A_set_right = false;
boolean B_set_right = false;

Adafruit_ILI9341 tft = Adafruit_ILI9341(LCD_PIN_CS, LCD_PIN_DC, LCD_PIN_SDI, LCD_PIN_SCK, LCD_PIN_RESET, LCD_PIN_SDO);
MPU9250_WE myMPU9250 = MPU9250_WE(MPU_ADDR);

void doEncoderALeft();
void doEncoderBLeft();
void doEncoderARight();
void doEncoderBRight();

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

  // Left nob
  pinMode(ENCODER_LEFT_PIN_A, INPUT);
  pinMode(ENCODER_LEFT_PIN_B, INPUT);
  pinMode(ENCODER_LEFT_PIN_BUTTON, INPUT);

  // Right nob
  pinMode(ENCODER_RIGHT_PIN_A, INPUT);
  pinMode(ENCODER_RIGHT_PIN_B, INPUT);
  pinMode(ENCODER_RIGHT_PIN_BUTTON, INPUT);


  // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(digitalPinToInterrupt(ENCODER_LEFT_PIN_A), doEncoderALeft, CHANGE);
  // encoder pin on interrupt 1 (pin 3)
  attachInterrupt(digitalPinToInterrupt(ENCODER_LEFT_PIN_B), doEncoderBLeft, CHANGE);

  // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(digitalPinToInterrupt(ENCODER_RIGHT_PIN_A), doEncoderARight, CHANGE);
  // encoder pin on interrupt 1 (pin 3)
  attachInterrupt(digitalPinToInterrupt(ENCODER_RIGHT_PIN_B), doEncoderBRight, CHANGE);

  Serial.begin(115200);
}

void doEncoderALeft(){
  // debounce
  if ( rotatingLeft ) delay (1);  // wait a little until the bouncing is done

  // Test transition, did things really change? 
  if( digitalRead(ENCODER_LEFT_PIN_A) != A_set_left ) {  // debounce once more
    A_set_left = !A_set_left;

    // adjust counter + if A leads B
    if ( A_set_left && !B_set_left ) 
      encoderPosLeft += 1;

    rotatingLeft = false;  // no more debouncing until loop() hits again
  }
}

void doEncoderBLeft(){
  if ( rotatingLeft ) delay (1);
  if( digitalRead(ENCODER_LEFT_PIN_B) != B_set_left ) {
    B_set_left = !B_set_left;
    //  adjust counter - 1 if B leads A
    if( B_set_left && !A_set_left ) 
      encoderPosLeft -= 1;

    rotatingLeft = false;
  }
}

void doEncoderARight(){
  // debounce
  if ( rotatingRight ) delay (1);  // wait a little until the bouncing is done

  // Test transition, did things really change? 
  if( digitalRead(ENCODER_RIGHT_PIN_A) != A_set_right ) {  // debounce once more
    A_set_right = !A_set_right;

    //adjust counter + if A leads B
    if ( A_set_right && !B_set_right ) 
      encoderPosRight += 1;

    rotatingRight = false;  // no more debouncing until loop() hits again
  }
}

void doEncoderBRight(){
  if ( rotatingRight ) delay (1);
  if( digitalRead(ENCODER_RIGHT_PIN_B) != B_set_right ) {
    B_set_right = !B_set_right;
    //  adjust counter - 1 if B leads A
    if( B_set_right && !A_set_right ) 
      encoderPosRight -= 1;

    rotatingRight = false;
  }
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

void screenLoop() {
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

void printLeftEncoder() {
  Serial.print("Left Encoder: ");
  Serial.println(digitalRead(ENCODER_LEFT_PIN_BUTTON));
  Serial.println(encoderPosLeft);
  lastReportedPosLeft = encoderPosLeft;
}

void printRightEncoder() {
  Serial.print("Right Encoder: ");
  Serial.println(digitalRead(ENCODER_RIGHT_PIN_BUTTON));
  Serial.println(encoderPosRight);
  lastReportedPosRight = encoderPosRight;
}

void loop(void) {
  printBno();
  printLeftEncoder();
  printRightEncoder();
  delay(500);
}

