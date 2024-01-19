#include "LEDWifiInterface.h"
#include "Timekeeper.h"
#include "LEDControls.h"

void test_alarm()
{
  Serial.println("This is the alarm that should be called");
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  wifi_setup();
  Timekeeper_setup();

  //Timekeeper_AlarmSet(Timekeeper_GetTime()+60,test_alarm);

  LEDControls_Setup();

}

void loop() {
  // put your main code here, to run repeatedly:

  wifi_loop();
  Timekeeper_loop();

  LEDControls_Loop();

}
