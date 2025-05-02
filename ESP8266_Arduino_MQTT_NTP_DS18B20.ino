#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>

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

static bool do_serial = false;

/*
  Following code slavishly copied from 
  https://www.emqx.com/en/blog/esp8266-connects-to-the-public-mqtt-broker
*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
WiFiClient espClient;
PubSubClient mqtt_client(espClient);
void connectToMQTTBroker(void) {
  while (!mqtt_client.connected()) {
      String client_id = "esp8266-client-" + String(WiFi.macAddress());
      if( do_serial) Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
      if (mqtt_client.connect(client_id.c_str(), "", "")) {
          Serial.println("Connected to MQTT broker");
          // mqtt_client.subscribe(mqtt_topic);          // Publish message upon successful connection
          mqtt_client.publish("ESP8266/test", "Hi MQTT I'm ESP8266 ^^");
      } else {
        if( do_serial)  {
          Serial.print("Failed to connect to MQTT broker, rc=");
          Serial.print(mqtt_client.state());
          Serial.println(" try again in 5 seconds");
        }
        delay(5000);
      }
  }
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void setup() {
  // Configure Serial for debugging
  if( do_serial) Serial.begin(115200);
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

  if( do_serial) Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    if( do_serial) Serial.print(".");
  }
  if( do_serial) {
    Serial.println();

    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
  }
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
    if( do_serial) Serial.printf("NTP time: %u\n\r", now);
    delay(500);
  }

  mqtt_client.setServer(mqtt_broker, 1883);
  //mqtt_client.setCallback(mqttCallback);
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
  if( do_serial) Serial.printf("Temperature at %u : %f.2 F, %f.2 C\n\r",
      now, sensors.getTempFByIndex(0), sensors.getTempCByIndex(0));
  connectToMQTTBroker();
  delay(1000);
/**********************************************************/


}
