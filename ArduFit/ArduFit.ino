#include <Arduino.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
// CONSTANTS
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display
// pin config
const int xpin = A2;                  // x-axis of the accelerometer
const int ypin = A1;                  // y-axis
const int zpin = A0;                  // z-axis (only on 3-axis models)
// constants
const float zVoltageOffset = 1.65;
const float accMax = 5;
// variables
int sitUps = 0;
int pushUps = 0;
int squads = 0;

int xValue = 0;
int yValue = 0;
int zValue = 0;

void updateStateScreen(){
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0,0,"     ArduFIT     ");
  u8x8.drawString(0, 3,"SitUps:    ");
  u8x8.drawString(0, 5,"PushUps:   ");
  u8x8.drawString(0, 7,"Squads:    ");
  u8x8.refreshDisplay();
}


void setup() {
  u8x8.begin();
  u8x8.setPowerSave(0);
  // initialize the serial communications:
  Serial.begin(9600);
  updateStateScreen();
  updateStatesOnScreen();
}

void loop() {
// ToDo:
// function getSensorData
// function convertToG // convert to m/s^2
// function isSitUp()
// function isPushUp();
// function isSquad();

  readValues();
   
  if(isSitUp())
  {
    sitUps = sitUps +1;
    updateStatesOnScreen();
  }
  
  serialPrintGValues();
  delay(100);
}

void readValues()
{
  xValue = analogRead(xpin);
  yValue = analogRead(ypin);
  zValue = analogRead(zpin);
}

void updateStatesOnScreen()
{
  String sitUpsString = String(sitUps);
  u8x8.setCursor(10,3);
  u8x8.print(sitUps);
  u8x8.setCursor(10,5);
  u8x8.print(pushUps);
  u8x8.setCursor(10,7);
  u8x8.print(squads);
  u8x8.refreshDisplay();
}

boolean isSitUp()
{
  if(calculateGforce(zValue) > 0.8)
  {
    return true;
  }
  else return false;
}

void serialPrintGValues()
{
  float xAcc = calculateGforce(xValue);
  Serial.print(xAcc);
  Serial.print("\t");
  float yAcc = calculateGforce(yValue);
  Serial.print(yAcc);
  Serial.print("\t");
  float zAcc = calculateGforce(zValue);
  Serial.print(zAcc);
  Serial.print("\t");
  Serial.println();
}

float calculateGforce(int value)
{
  float voltage = (float) (5/1024.0)*value;
  return (voltage - zVoltageOffset)*(accMax/zVoltageOffset);
}
