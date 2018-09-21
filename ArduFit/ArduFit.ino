#include <Wire.h>
#include <SeeedOLED.h>
/*
  ADXL3xx

  Reads an Analog Devices ADXL3xx accelerometer and communicates the
  acceleration to the computer. The pins used are designed to be easily
  compatible with the breakout boards from SparkFun, available from:
  http://www.sparkfun.com/commerce/categories.php?c=80

  The circuit:
  - analog 0: accelerometer self test
  - analog 1: z-axis
  - analog 2: y-axis
  - analog 3: x-axis
  - analog 4: ground
  - analog 5: vcc

  created 2 Jul 2008
  by David A. Mellis
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/ADXL3xx
*/
// we maybe should remove this^^

// these constants describe the pins. They won't change:
//const int groundpin = 16;             // analog input pin 4 -- ground
//const int powerpin = ;              // analog input pin 5 -- voltage
const int xpin = A0;                  // x-axis of the accelerometer
const int ypin = A1;                  // y-axis
const int zpin = A2;                  // z-axis (only on 3-axis models)


// counter for exercises
int sitUps = 0;
int pushUps = 0;
int squads = 0;

void updateStateScreen(){
  SeeedOled.setTextXY(0,0);
  SeeedOled.putString("     ArduFIT     "); //Print the String
  SeeedOled.setTextXY(3,0);         
  SeeedOled.putString("SitUps:    ");
  SeeedOled.putNumber(sitUps);
  SeeedOled.setTextXY(5,0);         
  SeeedOled.putString("PushUps:   ");
  SeeedOled.putNumber(pushUps);
  SeeedOled.setTextXY(7,0);         
  SeeedOled.putString("Squads:    ");
  SeeedOled.putNumber(squads);
}

void setup() {
  // display header
  Wire.begin();
  SeeedOled.init();  //initialze SEEED OLED display

  SeeedOled.clearDisplay();          //clear the screen and set start position to top left corner
  SeeedOled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();           //Set addressing mode to Page Mode
  SeeedOled.setTextXY(0,0);          //Set the cursor to Xth Page, Yth Column  
  SeeedOled.putString("ready?"); //Print the String

 
  // initialize the serial communications:
  Serial.begin(9600);
  updateStateScreen();
}

void loop() {
// ToDo:
// function getSensorData
// function convertToG // convert to m/s^2
// function isSitUp()
// function isPushUp();
// function isSquad();
 
  // print the sensor values:
  Serial.print(analogRead(xpin));
  // print a tab between values:
  Serial.print("\t");
  Serial.print(analogRead(ypin));
  // print a tab between values:
  Serial.print("\t");
  Serial.print(analogRead(zpin));
 
  Serial.println();
  // delay before next reading:
  delay(100);
}
