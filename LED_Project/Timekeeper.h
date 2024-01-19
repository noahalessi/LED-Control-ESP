#ifndef _Timekeeper_h
#define _Timekeeper_h

#include <Arduino.h>

typedef void(*alarm_cb)(void);

void Timekeeper_setup();
void Timekeeper_AlarmSet(unsigned long alarm_in,alarm_cb function);
unsigned long Timekeeper_GetTime();
String Timekeeper_GetFormmattedTime();
void Timekeeper_AlarmOff();
void Timekeeper_loop();

void local_time_update();


#endif
