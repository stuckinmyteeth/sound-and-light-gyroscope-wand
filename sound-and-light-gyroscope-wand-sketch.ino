// Below: Colors to use
int arrRgbColsNone[] = {0,0,0};
int arrRgbColsNotReady[] = {155,0,0};
int arrRgbColsReady[] = {155,155,0};

int arrRgbColsPitchPositive[] = {0,0,155};
int arrRgbColsPitchNegative[] = {0,112,155};

int arrRgbColsRollPositive[] = {27,0,155};
int arrRgbColsRollNegative[] = {112,0,155};

int arrRgbColsYawPositive[] = {155,255,0};
int arrRgbColsYawNegative[] = {155,70,0};


const float fltFreqTonePitchPositive = 349.228; //F4
const float fltFreqTonePitchNegative = 261.626; //C4

const float fltFreqToneRollPositive = 440.000; //A4
const float fltFreqToneRollNegative = 293.665; //D4

const float fltFreqToneYawPositive = 391.995; //G4
const float fltFreqToneYawNegative = 329.628; //E4


const int intTriggerDegreesPitch = 30;
const int intTriggerDegreesRoll = 45;
const int intTriggerDegreesYaw = 30;


/*

 Many thanks to the creators of the two libraries without which it would've been much more difficult to write this sketch:

 * Korneliusz Jarzebski's MPU6050 library: https://github.com/jarzebski/Arduino-MPU6050
 * Adafruit NeoPixel Library: https://github.com/adafruit/Adafruit_NeoPixel

*/

#include <Wire.h>
#include <MPU6050.h>
MPU6050 mpu;
// MPU6050 global vars - Timers
unsigned long ulTimer = 0;
const float fltTimeStep = 0.02;
// MPU6050 global vars - Pitch, Roll and Yaw values
float fltPitch = 0;
float fltRoll = 0;
float fltYaw = 0;


#include <Adafruit_NeoPixel.h>
const int pinNeoPixels = 3;
const int intNumNeoPixels = 5;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(intNumNeoPixels, pinNeoPixels, NEO_GRB + NEO_KHZ800);


float fltToneCurrent = 0;
const int pinSpeaker = 9;


int intNumConsecSameGyroMeasurements = 0;
String strPrevPitchRollYawVals = "";
String rtnStrPitchRollYawVals(float valsPitchRollYaw);


String strNeoPixelsCurrentRgbVal = "";
void setNeoPixelsAllOneColor(int rgbValues[]);


void setup()
{
  Serial.begin(115200);
  // Initialize NeoPixel lib
  pixels.begin();
  pixels.setBrightness(64);
  pixels.show();
  setNeoPixelsAllOneColor(arrRgbColsNotReady);
  // Initialize MPU6050
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    // Flashes LEDs between arrRgbColsNone and arrRgbColsNotReady as a heads up
    // in case the user is not watching the Arduino serial monitor.
    setNeoPixelsAllOneColor(arrRgbColsNone);
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
    setNeoPixelsAllOneColor(arrRgbColsNotReady);
  }
  mpu.calibrateGyro();
  mpu.setThreshold(1);
  setNeoPixelsAllOneColor(arrRgbColsReady);
}

void loop()
{
  ulTimer = millis();
  // Read normalized values
  Vector norm = mpu.readNormalizeGyro();
  // Calculate Pitch, Roll and Yaw
  fltPitch = fltPitch + norm.YAxis * fltTimeStep;
  fltRoll = fltRoll + norm.XAxis * fltTimeStep;
  fltYaw = fltYaw + norm.ZAxis * fltTimeStep;
  // Absolute values of Pitch, Roll, and Yaw for comparison purposes
  float absFltPitch = abs(fltPitch);
  float absFltRoll = abs(fltRoll);
  float absFltYaw = abs(fltYaw);
  // Act on Pitch, Roll, and Yaw values
  if (((absFltPitch > absFltRoll) && (absFltPitch > absFltYaw))
      && (absFltPitch > intTriggerDegreesPitch)) {
    Serial.print(" Pitch = ");
    Serial.println(fltPitch);
    if(fltPitch > 0) {
      setNeoPixelsAllOneColor(arrRgbColsRollPositive);
      playOneToneAtaTime(fltFreqTonePitchPositive);
    } else {
      setNeoPixelsAllOneColor(arrRgbColsRollNegative);
      playOneToneAtaTime(fltFreqTonePitchNegative);
    }
  }
  else if (((absFltRoll > absFltPitch) && (absFltRoll > absFltYaw))
           && (absFltRoll > intTriggerDegreesRoll)) {
    Serial.print(" Roll = ");
    Serial.println(fltRoll);
    if(fltRoll > 0) {
      setNeoPixelsAllOneColor(arrRgbColsPitchPositive);
      playOneToneAtaTime(fltFreqToneRollPositive);
    } else {
      setNeoPixelsAllOneColor(arrRgbColsPitchNegative);
      playOneToneAtaTime(fltFreqToneRollNegative);
    }
  }
  else if (((absFltYaw > absFltPitch) && (absFltYaw > absFltRoll))
           && (absFltYaw > intTriggerDegreesYaw)) {
    Serial.print(" Yaw = ");
    Serial.println(fltYaw);
    if(fltYaw > 0) {
      setNeoPixelsAllOneColor(arrRgbColsYawPositive);
      playOneToneAtaTime(fltFreqToneYawPositive);
    } else {
      setNeoPixelsAllOneColor(arrRgbColsYawNegative);
      playOneToneAtaTime(fltFreqToneYawNegative);
    }
  }
  else {
    setNeoPixelsAllOneColor(arrRgbColsReady);
    noTone(pinSpeaker);
    displayGyroMeasurements(fltPitch, fltRoll, fltYaw);
  }
  // Wait to full fltTimeStep period
  int intTimeToDelayMs = (fltTimeStep*1000) - (millis() - ulTimer);
  delay(intTimeToDelayMs);
}

void setNeoPixelsAllOneColor(int rgbValues[])
{
  String strThisColor = rtnStrRgbVals(rgbValues);
  if (strThisColor != strNeoPixelsCurrentRgbVal) {
    for(int i=0; i<intNumNeoPixels; i++){
      pixels.setPixelColor(i, pixels.Color(rgbValues[0], rgbValues[1], rgbValues[2]));
    }
    pixels.show();
    Serial.println("Set Neopixels to RGB color "+strThisColor);
    strNeoPixelsCurrentRgbVal = strThisColor;
  }
}

String rtnStrRgbVals(int rgbValues[])
{
    String strRgbVals = "(";
    strRgbVals += rgbValues[0];
    strRgbVals +=  ",";
    strRgbVals += rgbValues[1];
    strRgbVals +=  ",";
    strRgbVals += rgbValues[2];
    strRgbVals +=  ")";
    return strRgbVals;
}

void displayGyroMeasurements(float fltPitch, float fltRoll, float fltYaw)
{
    float valsPitchRollYaw[] = {fltPitch, fltRoll, fltYaw};
    String strPitchRollYawVals = rtnStrPitchRollYawVals(valsPitchRollYaw);
    if (strPitchRollYawVals == strPrevPitchRollYawVals) {
      if (intNumConsecSameGyroMeasurements == 0) {
        Serial.print('\n');
        Serial.print('*');
      } else if (((intNumConsecSameGyroMeasurements + 1) % 50) == 0) {
        Serial.println('*');
      } else {
        Serial.print('*');
      }
      intNumConsecSameGyroMeasurements++;
    } else {
      strPrevPitchRollYawVals = strPitchRollYawVals;
      if (intNumConsecSameGyroMeasurements > 0) {
        Serial.print('\n');
      }
      Serial.println(strPitchRollYawVals);
      intNumConsecSameGyroMeasurements = 0;
    }
}

String rtnStrPitchRollYawVals(float valsPitchRollYaw[])
{
    String strPitchRollYawVals = "Pitch: ";
    strPitchRollYawVals += valsPitchRollYaw[0];
    strPitchRollYawVals +=  ", Roll: ";
    strPitchRollYawVals += valsPitchRollYaw[1];
    strPitchRollYawVals +=  ", Yaw: ";
    strPitchRollYawVals += valsPitchRollYaw[2];
    return strPitchRollYawVals;
}

void playOneToneAtaTime(float fltToneNew)
{
  if (fltToneNew != fltToneCurrent) {
    noTone(pinSpeaker);
    fltToneCurrent = fltToneNew;
  }
  tone(pinSpeaker, fltToneNew, 100);
}

