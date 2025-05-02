#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <stdio.h>

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

int connectToMQTTBroker_and_publish(const char *topic, const char *payload)
{
  String client_id = "esp8266-client-" + String(WiFi.macAddress());
  if (do_serial)
    Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
  if (mqtt_client.connect(client_id.c_str(), "", ""))
  {
    if (do_serial)
      Serial.println("Connected to MQTT broker");
    mqtt_client.publish(topic, payload);
    mqtt_client.disconnect();
    return 0; // success
  }
  else
  {
    if (do_serial)
    {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
    }
    return mqtt_client.state();
  }
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void setup()
{
  // Configure Serial for debugging
  if (do_serial)
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

  if (do_serial)
    Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    if (do_serial)
      Serial.print(".");
  }
  if (do_serial)
  {
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
  while (now < 1746133868)
  {
    timeClient.update();
    now = timeClient.getEpochTime();
    if (do_serial)
      Serial.printf("NTP time: %llu\n\r", now);
    delay(500);
  }

  mqtt_client.setServer(mqtt_broker, 1883);
  // mqtt_client.setCallback(mqttCallback);
}

// buffer to build MQTT payload
#define payload_len 200
char payload[payload_len];
#define float_len 10
char float_buf[float_len];

void loop()
{
  // TODO: add code to periodically update NTP time - once/hour?
  /**********************************************************/
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures();

  // Serial.print("Celsius temperature: ");
  //  Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  // Serial.print(sensors.getTempCByIndex(0));
  time_t now = timeClient.getEpochTime();
  float temperature_f = sensors.getTempFByIndex(0);
  // dtostrf( float val, minimum field width, digits after decimal, buffer );
  //dtostrf( temperature_f, 5, 2, float_buf );
  sprintf(payload, 
    "{\"t\":%lld, \"temp\":%.2f, \"device\":\"DS18B20\"}",
    now, temperature_f);
  Serial.println(payload);
/*
  snprintf(payload, payload_len, 
    "{ \"t\":%ld, \"temp\": %s, \"device\":\"DS18B20\" }",
    now, float_buf);

  if (do_serial)
    Serial.printf("Temperature at %u (%d): %.2f F, '%s'\n\r",
                  now, now, temperature_f, float_buf);\

    // build JSON formatted payload
    // { "t":1746193325, "temp": 70.1, "device":"DS18B20" }
    snprintf(payload, payload_len, 
      "{ \"t\":%ld, \"temp\": %s, \"device\":\"DS18B20\" }",
      now, float_buf);
    if (do_serial)
      Serial.println(payload);
*/
  
  connectToMQTTBroker_and_publish(my_topic, payload);
  delay(10000);
  /**********************************************************/
}
