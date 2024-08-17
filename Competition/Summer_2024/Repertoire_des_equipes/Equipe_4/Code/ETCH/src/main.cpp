#include <Arduino.h>
/**
 * TCA9548 I2CScanner.ino -- I2C bus scanner for Arduino
 *
 * Based on https://playground.arduino.cc/Main/I2cScanner/
 *
 */
#include <BleMouse.h>
#include "Wire.h"
#include "SPI.h"
#include <Adafruit_ILI9341.h>
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

#define SCROLL_WHEEL_PIN_A 38
#define SCROLL_WHEEL_PIN_B 37

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480


volatile int encoderPosLeft = 0;  // a counter for the dial
unsigned int lastReportedPosLeft = 1;   // change management
static boolean rotatingLeft=false;      // debounce management

volatile int encoderPosRight = 0;  // a counter for the dial
unsigned int lastReportedPosRight = 1;   // change management
static boolean rotatingRight=false;      // debounce management

volatile int encoderPosScroll = 0;  // a counter for the dial
unsigned int lastReportedPosScroll = 1;   // change management
static boolean rotatingScroll=false;      // debounce management

// interrupt service routine vars
boolean A_set_left = false;              
boolean B_set_left = false;

boolean A_set_right = false;
boolean B_set_right = false;

boolean A_set_scroll = false;
boolean B_set_scroll = false;

Adafruit_ILI9341 tft = Adafruit_ILI9341(LCD_PIN_CS, LCD_PIN_DC, LCD_PIN_SDI, LCD_PIN_SCK, LCD_PIN_RESET, LCD_PIN_SDO);
MPU9250_WE myMPU9250 = MPU9250_WE(MPU_ADDR);
BleMouse bleMouse = BleMouse("Etch A Sketch", "Hackermouse", 50);;

int scrollDelta = 0;
int leftDelta = 0;
int rightDelta = 0;

int cursorX = 0;
int cursorY = 0;

void doEncoderALeft();
void doEncoderBLeft();
void doEncoderARight();
void doEncoderBRight();
void doEncoderAScroll();
void doEncoderBScroll();

unsigned long lastMousePing = 0;
unsigned long screenDelta = 0;
unsigned long bnoDelta = 0;
unsigned long shakingStart = 0;

void setup() {
  // Screen setup
  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(ILI9341_BLACK);

  // Set correct pos, higher to
  encoderPosLeft = 120 + (tft.width() * 10);
  encoderPosRight = 160 + + (tft.height() * 10);

  // TODO display a splash screen, we are loading

  Serial.begin(115200);
  // BNO stuff
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
  attachInterrupt(digitalPinToInterrupt(ENCODER_LEFT_PIN_A), doEncoderALeft, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_LEFT_PIN_B), doEncoderBLeft, CHANGE);


  // Right nob
  pinMode(ENCODER_RIGHT_PIN_A, INPUT);
  pinMode(ENCODER_RIGHT_PIN_B, INPUT);
  pinMode(ENCODER_RIGHT_PIN_BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCODER_RIGHT_PIN_A), doEncoderARight, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_RIGHT_PIN_B), doEncoderBRight, CHANGE);


  // Scroll
  pinMode(SCROLL_WHEEL_PIN_A, INPUT);
  pinMode(SCROLL_WHEEL_PIN_B, INPUT);
  attachInterrupt(digitalPinToInterrupt(SCROLL_WHEEL_PIN_A), doEncoderAScroll, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SCROLL_WHEEL_PIN_B), doEncoderBScroll, CHANGE);  

  // Ready to roll, start BLE
  Serial.println("Starting BLE work!");
  bleMouse.begin();
}

// Interrupts go here

void doEncoderAScroll(){
  // debounce
  if ( rotatingScroll ) delay (1);  // wait a little until the bouncing is done

  // Test transition, did things really change? 
  if( digitalRead(SCROLL_WHEEL_PIN_A) != A_set_scroll ) {  // debounce once more
    A_set_scroll = !A_set_scroll;

    // adjust counter + if A leads B
    if ( A_set_scroll && !B_set_scroll ) 
      encoderPosScroll += 1;

    rotatingScroll = false;  // no more debouncing until loop() hits again
  }
}

void doEncoderBScroll(){
  if ( rotatingScroll ) delay (1);
  if( digitalRead(SCROLL_WHEEL_PIN_B) != B_set_scroll ) {
    B_set_scroll = !B_set_scroll;
    //  adjust counter - 1 if B leads A
    if( B_set_scroll && !A_set_scroll ) 
      encoderPosScroll -= 1;

    rotatingScroll = false;
  }
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

void printScrollEncoder() {
  Serial.print("Scroll Encoder: ");
  Serial.println(encoderPosScroll);
  lastReportedPosScroll = encoderPosScroll;
}

int calcLeftDelta() {
  int delta = encoderPosLeft - lastReportedPosLeft;
  lastReportedPosLeft = encoderPosLeft;
  return delta;
}

int calcRightDelta() {
  int delta = encoderPosRight - lastReportedPosRight;
  lastReportedPosRight = encoderPosRight;
  return delta;
}

int calcScrollDelta() {
  int delta = encoderPosScroll - lastReportedPosScroll;
  lastReportedPosScroll = encoderPosScroll;
  return delta;
}

int applyMouseAcceleration(int delta, int multiplier = 1) {
  const int maxAcceleration = 150;

  if(delta > 0) {
    return min(delta * 2, maxAcceleration) * multiplier;
  } else {
    return max(delta * 2, -maxAcceleration) * multiplier;
  }
}

void loop(void) {
  if (millis() - lastMousePing > 10) {
    scrollDelta = calcScrollDelta();
    leftDelta = calcLeftDelta();
    rightDelta = calcRightDelta();

    if(bleMouse.isConnected()) {
      bleMouse.move(
        applyMouseAcceleration(rightDelta, 4),
        applyMouseAcceleration(-leftDelta, 4),
        applyMouseAcceleration(scrollDelta)
        );

      // Handle LEFT_CLICK
      if (bleMouse.isPressed(MOUSE_LEFT) && digitalRead(ENCODER_LEFT_PIN_BUTTON) == HIGH) {
        bleMouse.release(MOUSE_LEFT);
      } else if(!bleMouse.isPressed(MOUSE_LEFT) && digitalRead(ENCODER_LEFT_PIN_BUTTON) == LOW) {
        bleMouse.press(MOUSE_LEFT);
      }
      
      // Handle RIGHT_CLICK
      if (bleMouse.isPressed(MOUSE_RIGHT) && digitalRead(ENCODER_RIGHT_PIN_BUTTON) == HIGH) {
        bleMouse.release(MOUSE_RIGHT);
      } else if(!bleMouse.isPressed(MOUSE_RIGHT) && digitalRead(ENCODER_RIGHT_PIN_BUTTON) == LOW) {
        bleMouse.press(MOUSE_RIGHT);
      }

      lastMousePing = millis();
    }

    // screen stuff
    if (millis() - screenDelta > 42) { // estimated 24fps

      
      tft.drawPixel(abs(encoderPosLeft) % tft.width(), abs(encoderPosRight) % tft.height(), ILI9341_WHITE);

      screenDelta = millis();
    }

    if (millis() - bnoDelta > 100) { // 4 times per second
      xyzFloat gValue = myMPU9250.getGValues();
      float resultantG = myMPU9250.getResultantG(gValue);

      if (resultantG > 1.1 || resultantG < 0.9) {
        if (shakingStart == 0) {
          shakingStart = millis();
        } else if (millis() - shakingStart > 750) {
          tft.fillScreen(ILI9341_BLACK);
          shakingStart = 0;
        }
      } else {
        shakingStart = 0;
      }
      bnoDelta = millis();
    }
  }
}