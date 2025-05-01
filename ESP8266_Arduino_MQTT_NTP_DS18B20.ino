#include <OneWire.h>
#include <DallasTemperature.h>

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


void setup() {
  // Configure Serial for debugging
  Serial.begin(115200);
/**********************************************************/
  // Start up the library
  sensors.begin();
/**********************************************************/

}

void loop() {
/**********************************************************/
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures(); 
  
  Serial.print("Celsius temperature: ");
  // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  Serial.print(sensors.getTempCByIndex(0)); 
  Serial.print(" - Fahrenheit temperature: ");
  Serial.println(sensors.getTempFByIndex(0));
  delay(1000);
/**********************************************************/

}
