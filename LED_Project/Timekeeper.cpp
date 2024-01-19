#include "Timekeeper.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

#define REASON_LIMIT 2400

unsigned long current_time;
bool update_time_bool;

unsigned long alarm_time;
bool alarm_on;
bool alarm_called;
bool alarm_daypass;

unsigned long last_millis;
unsigned long this_millis;

int internal_reasons;

alarm_cb alarm_to_call;

void local_time_update()
{
  timeClient.update();
  internal_reasons = 0;
  String formattedTime = timeClient.getFormattedTime();
  current_time = timeClient.getHours()*3600+timeClient.getMinutes()*60+timeClient.getSeconds();

  Serial.print("NTP Time: ");
  Serial.println(current_time);
  Serial.println(formattedTime);
}

void Timekeeper_setup(){
  timeClient.begin();
  timeClient.setTimeOffset(-18000);
  local_time_update();

  alarm_on = false;
  alarm_called = false;
  alarm_daypass = false;
  alarm_to_call = 0;

  last_millis = millis();
  this_millis = last_millis;
}

void Timekeeper_AlarmSet(unsigned long alarm_in,alarm_cb function)
{
  alarm_time = alarm_in;
  if(current_time > alarm_time)
  {
    alarm_daypass = true;
  }else
  {
    alarm_daypass = false;
  }
  alarm_to_call = function;
  alarm_on = true;
  alarm_called = false;
}

unsigned long Timekeeper_GetTime()
{
  return current_time;
}

String Timekeeper_GetFormmattedTime()
{
  int hrs = current_time/3600;
  int mins = (current_time-hrs*3600)/60;
  
  String return_time = String(String(hrs) + ":" + String(mins));

  return return_time;
}

void Timekeeper_AlarmOff()
{
  alarm_on = false;
  alarm_to_call = 0;
  alarm_time = 0;
}

void Timekeeper_loop(){

  this_millis = millis();

  if(this_millis - last_millis > 1000 && internal_reasons < REASON_LIMIT)
  {
    internal_reasons++;
    current_time += (this_millis - last_millis)/1000;
    last_millis = this_millis;
    //Serial.print("Internal Time: ");
    //Serial.println(current_time);
  }else if (internal_reasons >= REASON_LIMIT)
  {
    local_time_update();
  }

  if(alarm_on && !alarm_daypass && current_time > alarm_time)
  {
    if(!alarm_called && alarm_to_call)
    {
      Serial.println("Calling Alarm");
      alarm_to_call ();
      alarm_called = true;
    }
    
  }else if(alarm_on && current_time < alarm_time)
  {
    alarm_called = false;
    if(alarm_daypass)
    {
      alarm_daypass = false;
    }
  }
  
}
