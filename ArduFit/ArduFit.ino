#include <Arduino.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display

// these constants describe the pins. They won't change:
//const int groundpin = 16;             // analog input pin 4 -- ground
//const int powerpin = ;              // analog input pin 5 -- voltage
const int xpin = A0;                  // x-axis of the accelerometer
const int ypin = A1;                  // y-axis
const int zpin = A2;                  // z-axis (only on 3-axis models)

const float zVoltageOffset = 1.65;
const float accMax = 5;


// counter for exercises
int sitUps = 0;
int pushUps = 0;
int squads = 0;

void updateStateScreen(){
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0,0,"     ArduFIT     "); //Print the String
  u8x8.drawString(3,0,"SitUps:    "+sitUps);
  u8x8.drawString(5,0,"PushUps:    "+pushUps);
  u8x8.drawString(7,0,"Squads:    "+squads);
  u8x8.refreshDisplay();
  delay(2000);
}


void setup() {
   /* U8g2 Project: SSD1306 Test Board */
  //pinMode(10, OUTPUT);
  //pinMode(9, OUTPUT);
  //digitalWrite(10, 0);
  //digitalWrite(9, 0);   
  
  /* U8g2 Project: KS0108 Test Board */
  //pinMode(16, OUTPUT);
  //digitalWrite(16, 0);  
  
  u8x8.begin();
  u8x8.setPowerSave(0);
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

  int value = analogRead(zpin);
  Serial.print(value);
 
  float voltage = (float) (5/1024.0)*value;
  float acc = (voltage - zVoltageOffset)/zVoltageOffset * accMax;
    
  Serial.print("in G: ");
  Serial.println(acc);
  // delay before next reading:
  delay(100);
}
