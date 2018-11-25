#include <Arduino.h>
#include <U8x8lib.h>
#include <Math.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

// CONSTANTS
// pin config
const int XPIN = A2;                  // x-axis of the accelerometer
const int YPIN = A1;                  // y-axis
const int ZPIN = A0;                  // z-axis (only on 3-axis models)
// voltage constants
const float zVoltageOffset = 1.65;
const float accMax = 5;

// variables
int sitUps = 1;
int pushUps = 2;
int squads = 0;
// sensor data
int xValue = 0;
int yValue = 0;
int zValue = 0;

// state change variables
const String states[] = {"SitUp", "PushUp", "Squad"};
String state;
const float stateChangeDifference = 1000; // minimal time a state is active
float lastStateChangeTime = millis();
int tmpChangedStateCount = 0; // to be increased during ON state if exercise consists of OFF-ON-OFF (or UP-DOWN-UP or DOWN-UP-DOWN) procedure

// screen variables
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display
const uint8_t FONT = u8x8_font_chroma48medium8_r;


// ################## SETUP ##################
void setup() {
  Serial.begin(115200);

  initScreen();
}

// ################## LOOP ##################
void loop() {

  readValues();

  decideState();

  countExercises();

  //markCurrentStateOnScreen();
  updateStatesOnScreen();
  serialPrintGValues();
  delay(1);
}


// ################## ANALOG READ ##################
void readValues()
{
  xValue = analogRead(XPIN);
  yValue = analogRead(YPIN);
  zValue = analogRead(ZPIN);

}


// ################## DATA PROCESSING ##################
void decideState()
{

  if(calculateGforce(zValue) > (float)0.8)
  { // SitUps
    lastStateChangeTime = millis();
    state = states[0];
  }

  if(calculateGforce(zValue) < (float)(-0.6))
  { // PushUps
    lastStateChangeTime = millis();
    state = states[1];
  }

  if(calculateGforce(yValue) > (float)0.9)
  { // Squads
    // only change state if standing up for some time
    if (millis() - lastStateChangeTime > stateChangeDifference )
    {
      lastStateChangeTime = millis();
      state = states[2];
    }
  }
}

void countExercises()
{
  if(state == states[0])
  { // SitUp
    if(calculateGforce(yValue) > (float)0.8)
    { // Up
      tmpChangedStateCount++;
    }
    if(calculateGforce(zValue) > (float)0.8 && tmpChangedStateCount != 0)
    { // Down
      lastStateChangeTime = millis();
      tmpChangedStateCount = 0;
      sitUps++;
    }
  }
  if(state == states[1])
  { // PushUp
    if(calculateGforce(zValue) < (float)-1.1)
    { // Down
      tmpChangedStateCount++;
    }
    if(calculateGforce(zValue) < (float)0.6 && calculateGforce(zValue) > (float)-1.1 && tmpChangedStateCount != 0)
    { // Up
      lastStateChangeTime = millis();
      tmpChangedStateCount = 0;
      pushUps++;
    }
  }
  if(state == states[2])
  { // Squad
    if(calculateGforce(yValue) > (float)1.2)
    { // Down
      tmpChangedStateCount++;
    }
    if(calculateGforce(yValue) > (float)0.8 && calculateGforce(yValue) < (float)1.2 && tmpChangedStateCount != 0)
    {
      lastStateChangeTime = millis();
      tmpChangedStateCount = 0;
      squads++;
    }

  }
}

float calculateGforce(int value)
{
  float voltage = (float) (5/1024.0)*value;
  return (voltage - zVoltageOffset)*(accMax/zVoltageOffset);
}


// ################## SCREEN ##################

void initScreen()
{
  // initialize screen
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(FONT);
  updateStateScreen();
  updateStatesOnScreen();
}

void updateStateScreen()
{
  u8x8.drawString(0,0,"     ArduFIT     ");
  u8x8.drawString(0, 3,"SitUps:    ");
  u8x8.drawString(0, 5,"PushUps:   ");
  u8x8.drawString(0, 7,"Squads:    ");
  u8x8.refreshDisplay();
}

void markCurrentStateOnScreen()
{
  u8x8.setInverseFont(FONT);
  u8x8.setCursor(7,3);
  u8x8.print("*");
  u8x8.setCursor(7,5);
  u8x8.print("*");
  u8x8.setCursor(7,7);
  u8x8.print("*");

  u8x8.setFont(FONT);
  if(state == states[0])
  {
    u8x8.setCursor(7,3);
    u8x8.print("*");
  }
  if(state == states[1])
  {
    u8x8.setCursor(7,5);
    u8x8.print("*");
  }
  if(state == states[2])
  {
    u8x8.setCursor(7,7);
    u8x8.print("*");
  }


  u8x8.refreshDisplay();
}

void updateStatesOnScreen()
{
  u8x8.setFont(FONT);
  u8x8.setCursor(10,3);
  u8x8.print(sitUps);
  u8x8.setCursor(10,5);
  u8x8.print(pushUps);
  u8x8.setCursor(10,7);
  u8x8.print(squads);
  u8x8.refreshDisplay();
}


// ################## SERIAL ##################
void serialPrintGValues()
{
  //Serial.print(state);
  //Serial.print("\t");
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
