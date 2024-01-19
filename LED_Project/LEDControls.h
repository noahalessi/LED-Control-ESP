#ifndef _LEDControls_h
#define _LEDControls_h

#include <Arduino.h>

void LEDControls_Setup();

void LEDControls_Off();

void LEDControls_SetColor(uint8_t red_val, uint8_t green_val,uint8_t blue_val);

void LEDControls_SetBrightness(uint8_t brightness);

void LEDControls_Loop();

void LEDSunriseStart();
void LEDSunriseStop();

#endif
