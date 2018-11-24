#include <Arduino.h>
#include <U8x8lib.h>

#include <Math.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

// CONSTANTS
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display
// pin config
const int XPIN = A2;                  // x-axis of the accelerometer
const int YPIN = A1;                  // y-axis
const int ZPIN = A0;                  // z-axis (only on 3-axis models)
// voltage constants
const float zVoltageOffset = 1.65;
const float accMax = 5;
// absolute value of the acceleromation
float acc;
// variables
int sitUps = 0;
int pushUps = 0;
int squads = 0;
// sensor data
int xValue = 0;
int yValue = 0;
int zValue = 0;
// mean filter variables
const int numReadings = 200;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

// state change variables
const float stateChangeDifference = 10000; // 10 seconds
float lastStateChangeTime = millis();

// squad sample data
float squadSample[95] ={0.18,0.18,0.21,0.18,0.18,0.19,0.15,0.21,0.16,0.18,0.16,0.18,0.16,0.16,0.18,0.16,0.16,0.18,0.12,0.10,-0.03,-0.10,-0.01,-0.01,0.00,-0.19,-0.31,-0.43,-0.34,-0.37,-0.37,-0.32,-0.32,-0.32,-0.24,-0.31,-0.31,-0.32,-0.34,-0.31,-0.34,-0.31,-0.25,-0.22,-0.21,-0.13,-0.12,-0.01,-0.04,-0.01,-0.06,-0.04,-0.04,-0.03,-0.06,-0.06,0.05,0.08,0.09,-0.21,-0.07,-0.06,-0.10,0.08,0.10,0.09,0.05,0.00,0.02,0.15,0.15,-0.12,0.03,0.16,0.05,0.06,0.06,0.40,0.05,0.06,0.05,-0.01,0.09,0.05,0.05,0.10,0.03,0.00,0.15,-0.01,0.08,-0.04,0.22,-0.06,0.18};


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
  Serial.begin(115200);
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0; // initialize array
  }
  updateStateScreen();
  updateStatesOnScreen();
}

void loop() {

  readValues();

  if(isSitUp())
  {
    sitUps = sitUps +1;
  }
  if (isSquad())
  {
    squads = squads +1;
  }
  if (isPushUp())
  {
    pushUps = pushUps +1;
  }

  updateStatesOnScreen();
  //serialPrintGValues();
  printCorrelation();
  delay(1);
}

void readValues()
{
  xValue = analogRead(XPIN);
  yValue = analogRead(YPIN);
  zValue = analogRead(ZPIN);

  // calculate absolute Acceleraton
  acc = sqrt(square(xValue) + square(yValue));
  acc = sqrt(square(acc) + square(zValue));
}

void updateStatesOnScreen()
{
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
  if(calculateGforce(zValue) > (float)0.8)
  {
    lastStateChangeTime = millis();
    return true;
  }
  return false;
}

boolean isSquad()
{
  if(calculateGforce(yValue) > (float)0.9)
  {
    // only change state if standing up for some time
    if (millis() - lastStateChangeTime > stateChangeDifference )
    {
      return true;
    }
  }
  return false;
}

boolean isPushUp()
{
  if(calculateGforce(zValue) < (float)(-0.6))
  {
    return true;
  }
  return false;
}

void printCorrelation()
{
  float sampleLength =  sizeof(squadSample);
  int dataLength = numReadings;
  float result[dataLength];
  
  for (int i = 0; i < dataLength - sampleLength; i++) {
    result[i] = 0;
    Serial.print((float)squadSample[i]);
    for (int j = 0; j < sampleLength; j++) {
      result[i] += readings[i+j] * squadSample[j];      
    }
  }

  float maximum = 0;
  for ( int i = 0; i < dataLength; i++) {
    if(maximum < result[i] ) {maximum = result[i];}
  }
  //Serial.println(maximum);
  Serial.println();
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
  Serial.print(acc/calculateMeanValue(acc));
  //Serial.print("\t");
  //Serial.println(acc- calculateMeanValue(acc));
  Serial.println();
  
}

float calculateGforce(int value)
{
  float voltage = (float) (5/1024.0)*value;
  return (voltage - zVoltageOffset)*(accMax/zVoltageOffset);
}

float calculateMeanValue(int value)
{
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = value;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  // send it to the computer as ASCII digits
  //Serial.println(average);
  return average;
}
