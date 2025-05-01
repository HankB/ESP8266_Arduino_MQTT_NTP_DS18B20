#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "secrets.h"

// Following code slavishly copied from 
// https://randomnerdtutorials.com/guide-for-ds18b20-temperature-sensor-with-arduino/

/**********************************************************/
// Data wire is conntec to the Arduino digital pin 4
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);
/**********************************************************/

/*
  Following code slavishly copied from 
  https://randomnerdtutorials.com/esp8266-nodemcu-date-time-ntp-client-server-arduino/
*/

/*========================================================*/
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
/*========================================================*/


void setup() {
  // Configure Serial for debugging
  Serial.begin(115200);
/**********************************************************/
  // Start up the library
  sensors.begin();
/**********************************************************/

/* Following slavishly copied from
   https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html
// This code requires that you provide "secrets.h" with something like

const char* ssid="myssid";
const char* password="my_password"

   which I suggest you do *not* check into Github etc.
*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  /*========================================================*/
  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // We don't care, using time_t since Jan 1, 1970
  timeClient.setTimeOffset(0);
  /*========================================================*/

  time_t now = 0;
  while(now < 1746133868)
  {
    timeClient.update();
    now = timeClient.getEpochTime();
    Serial.printf("NTP time: %u\n\r", now);
    delay(500);
  }

}

void loop() {
  // TODO: add code to periodically update NTP time - once/hour?
/**********************************************************/
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures(); 
  
  //Serial.print("Celsius temperature: ");
  // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  //Serial.print(sensors.getTempCByIndex(0)); 
  time_t now = timeClient.getEpochTime();
  Serial.printf("Temperature at %u : %f.2 F, %f.2 C\n\r",
      now, sensors.getTempFByIndex(0), sensors.getTempCByIndex(0));
  delay(1000);
/**********************************************************/


}
