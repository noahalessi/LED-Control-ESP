#include "LEDControls.h"

#include "Timekeeper.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 5

#define SUNRISE_TIME (300)
#define SUNRISE_STEPS (71)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(150, PIN, NEO_GRB + NEO_KHZ800);

uint8_t Brightness;
bool SunriseOngoing = false;

unsigned long SunriseStart = 0;

void LEDControls_SetBrightness(uint8_t brightness)
{
  Brightness = brightness;
  strip.setBrightness(Brightness);
}

void LEDControls_Setup()
{
  strip.begin();
  Brightness = 100;
  strip.setBrightness(Brightness);
  strip.show();
}

void LEDControls_Off()
{
  for(uint16_t i=0; i<strip.numPixels(); i++) 
  {
    strip.setPixelColor(i, 0);
  }
  strip.show();
  LEDSunriseStop();
}

void LEDControls_SetColor(uint8_t red_val, uint8_t green_val,uint8_t blue_val)
{
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i,strip.Color(red_val,green_val,blue_val));
  }
  strip.show();
}

void LEDSunriseStart()
{
  SunriseStart = Timekeeper_GetTime();
  SunriseOngoing = true;
}

void LEDSunriseCalcSet()
{
  float x_val = (float)(Timekeeper_GetTime() - SunriseStart)/SUNRISE_TIME*SUNRISE_STEPS;

  if(x_val < SUNRISE_STEPS)
  {
    float red_val = 2.913*x_val+45.92;
    float green_val = 1.417*x_val+26.564;
    float blue_val = -0.7251*x_val+122.45;
  
    uint8_t red_byte = (uint8_t)red_val;
    uint8_t green_byte = (uint8_t)green_val; 
    uint8_t blue_byte = (uint8_t)blue_val;  
  
    LEDControls_SetBrightness(red_byte);
    LEDControls_SetColor(red_byte,green_byte,blue_byte);
  }else
  {
    LEDSunriseStop();
  }
  
}

void LEDSunriseStop()
{
  SunriseStart = 0;
  SunriseOngoing = false;
}

void LEDControls_Loop()
{
  if(SunriseOngoing)
  {
    LEDSunriseCalcSet();
  }
}
