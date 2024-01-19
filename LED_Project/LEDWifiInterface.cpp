#include "LEDWifiInterface.h"

#include "Timekeeper.h";
#include "LEDControls.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "Douglords Castle 2.4"
#define STAPSK "Douglord1"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int led = LED_BUILTIN;

void handleRoot() {
  digitalWrite(led, 1);
  
  String form_start = "<html>\
  <head>\
    <title>ESP8266 Web Server POST handling</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Set Light Color /postcolor/</h1><br>\
    <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/postcolor/\">\
      <label for=\"brightness\">Brightness (1 - 255):</label>\
      <input type=\"number\" id=\"brightness\" name=\"brightness\" min=\"1\" max=\"255\">\
      <input type=\"checkbox\" id=\"power1\" name=\"power1\" value=\"power1\">\
      <label for=\"power1\"> Power</label><br>\
      <input type=\"color\" id=\"favcolor\" name=\"favcolor\"><br>\
    <label for=\"favcolor\"> Color</label><br>\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
  <h1>Set Alarm Time /postalarm/</h1><br>\
    <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/postalarm/\">\
    <label for=\"alarm\">Select alarm time:</label>\
    <input type=\"time\" id=\"alarm\" name=\"alarm\">\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
  <h1>Update Time /updatetime/</h1><br>\
    <form method=\"get\" action=\"/updatetime/\">\
    <input type=\"submit\" value=\"Update Time\">\
    </form>\
  <h1>Current Time: ";

  String form_end = "</h1><br>\
  </body>\
  </html>";

  String postForms = String(form_start+Timekeeper_GetFormmattedTime()+form_end);
  server.send(200, "text/html", postForms);
  digitalWrite(led, 0);
}

void handleColor() {
  if (server.method() != HTTP_POST) {
    digitalWrite(led, 1);
    server.send(405, "text/plain", "Method Not Allowed");
    digitalWrite(led, 0);
  } else {
    digitalWrite(led, 1);
    String message = "POST color was:\n";

    bool power_bool = false;
    String color_code;

    uint8_t brightness = 0;
    
    for (uint8_t i = 0; i < server.args(); i++) 
    { 
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; 
      if(server.argName(i) == "favcolor")
      {
        color_code = server.arg(i);
      }

      if(server.argName(i) == "power1")
      {
        power_bool = true;
      }

      if(server.argName(i) == "brightness")
      {
        brightness = (uint8_t)server.arg(i).toInt();
      }
      }

    if(power_bool)
    {
      char color_buffer[3];
      memset(color_buffer,'\0',3);
      color_code.substring(1,3).toCharArray(color_buffer,3);
      uint8_t red =(uint8_t)strtol(color_buffer,NULL,16);
      
      memset(color_buffer,'\0',3);
      color_code.substring(3,5).toCharArray(color_buffer,3);
      uint8_t green =(uint8_t)strtol(color_buffer,NULL,16);
      
      memset(color_buffer,'\0',3);
      color_code.substring(5,7).toCharArray(color_buffer,3);
      uint8_t blue =(uint8_t)strtol(color_buffer,NULL,16);

      String print_string = "Power on, Color is: " + color_code + "\n" + "Red: " + red + "Green: " + green + "Blue: " + blue;
      
      Serial.println(print_string);

      LEDControls_SetColor(red, green,blue);
      LEDControls_SetBrightness(brightness);

      //strip.setBrightness(brightness);
      //led_set_color(red,green,blue);
    }else
    {
      Serial.println("Power off");
      LEDControls_Off();
    }
    server.send(200, "text/plain", message);
    digitalWrite(led, 0);
  }
}

void handleAlarm(){
  if (server.method() != HTTP_POST) {
    digitalWrite(led, 1);
    server.send(405, "text/plain", "Method Not Allowed");
    digitalWrite(led, 0);
  } else {
    String message;
    String alarm_time_string;
    unsigned long alarm_time_value = 0;
    for (uint8_t i = 0; i < server.args(); i++) 
    { 
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
      if (server.argName(i) == "alarm")
      {
        alarm_time_string = server.arg(i);
      }
    }

    alarm_time_value = (alarm_time_string.substring(0,2).toInt()*60+alarm_time_string.substring(3).toInt())*60;

    Serial.print("String: ");
    Serial.print(alarm_time_string);
    Serial.print(", Value: ");
    Serial.println(alarm_time_value);

    Timekeeper_AlarmSet(alarm_time_value,LEDSunriseStart);

    server.send(200, "text/plain", message);
  }
  
}

void handleUpdate()
{
  if(server.method() != HTTP_GET)
  {
    digitalWrite(led, 1);
    server.send(405, "text/plain", "Method Not Allowed");
    digitalWrite(led, 0);
  }else
  {
    local_time_update();
    String message = String("Updated time: " + Timekeeper_GetFormmattedTime());;
    server.send(200, "text/plain", message);
  }
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) { message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void wifi_setup() {
  // put your setup code here, to run once:

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }

  server.on("/", handleRoot);

  server.on("/postcolor/",handleColor);
  server.on("/postalarm/",handleAlarm);
  server.on("/updatetime/",handleUpdate);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");


}

void wifi_loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}
