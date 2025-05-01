# ESP8266 Arduino MQTT NTP DS18B20

## Motivation

It has been a struggle getting the DSD18B20 temperature sensor working with the ESP8266-RTOS-SDK tool chain so I am regressing to the Arduino tool chain and libraries (which use the ESP8266-RTOS-SDK under the covers.) My intent is too use VS Code for editing and the `arduino-cli` to build and flash. (Development is on Linux and using the `bash` shell.)

## 2025-05-01 Build starting point

* <https://gist.github.com/adi-g15/de41e96079a5b63045e86dc7c8c5c87e> 


```text
cat << EOF > .cli-config.yml
board_manager:
  additional_urls:
    - http://arduino.esp8266.com/stable/package_esp8266com_index.json
EOF
arduino-cli core install esp8266:esp8266 --config-file ./.cli-config.yml
arduino-cli board list
```

No joy.

```text
hbarta@olive:~/Programming/Arduino/ESP8266_Arduino_MQTT_NTP_DS18B20$ arduino-cli board list
Port         Protocol Type              Board Name FQBN Core
/dev/ttyUSB0 serial   Serial Port (USB) Unknown

hbarta@olive:~/Programming/Arduino/ESP8266_Arduino_MQTT_NTP_DS18B20$ 
```

Start with the IDE (flatpak) and choose the board `LOLIN(WEMOS) D1 Mini` and build successfully. There is a fair bit of text output to the console (since I launched the IDE from the command line.) and apparently the board ID is `esp8266:esp8266:d1_mini_clone`

```text
arduino-ide_2.3.6_Linux_64bit.AppImage ESP8266_Arduino_MQTT_NTP_DS18B20.ino
```

## 2025-05-01 retry CLI

```text
arduino-cli compile --fqbn esp8266:esp8266:d1_mini_clone ./ # build
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp8266:esp8266:d1_mini_clone ./ # flash
minicom --device /dev/ttyUSB0 # monitor
```

## Status

* 2025-05-01 First working build using the Arduino IDE.

## Next

* 2025-05-01 add support for DS18B20

## Hardware

* "WeMos D1 Mini" as near as I can tell - Amazon listing is for "KeeYees ESP8266 ESP-12F Mini WLAN WiFi Development Board 4M Bytes for Arduino for NodeMcu for WeMos." I bought 5 and that was a bit of a mistake because the ESP32 is much better suypported and doesn't cost that much more.
* Dallas DS18B20 temperature sensors, bnuth TO-92 packages and the waterproof probe style. Both have three connections and behave the same as far as the S/W is concerned.
* 4.7 K ohm resistors to pull the data line up to the 3V3 connector.

Connections are as follows:

|DS18B20|ESP8266|Note|
|---|---|---|
|GND|G|ground|
|VCC|3V3|Canb power from 5V but should use a voltage divider for the Data input to the ESP8266.|
|DATA|D4|This is called out as pin 2 in the S/W. I do not know why.|

## Errata

* Serial baud rate. The bootloader starts at 74880 and the Arduino IDE supports this. The `minicom` program seems not to (and defaults to ). Choose a baud rate accordingly.