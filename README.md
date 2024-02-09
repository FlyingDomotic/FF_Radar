# FF_Radar
 Integration of LD2450 radar module into HTTP, MQTT and/or Domoticz

## What's for?

This code reads integrates LD2450 radar into HTTP, MQTT and/or Domoticz. It can differentiate between detected targets (even not moving) and moving targets. Potition, speed and resolution of each target are available.

It send MQTT messages at regular interval to give number of target detected (up to 3), count of moving targets, and position (x/y), speed and resolution of each target, moving or not. Module can also return the same JSON message when asked to http://<module ip or name>/json/values

It can also send to Domoticz either flag and/or count of both detected target and moving target, each time something changes.

## Prerequisites

You must have either Arduino IDE or VSCodium (free version of Visual Studio) installed in order to compile the code.

## Installation

Clone repository somewhere on your disk.
```
cd [where_you_want_to_install_it]
git clone https://github.com/FlyingDomotic/FF_Radar.git FF_Radar
```

## Update

Go to code folder and pull new version:
```
cd [where_you_installed_FF_Radar]
git pull
```

Note: if you did any changes to files and `git pull` command doesn't work for you anymore, you could stash all local changes using:
```
git stash
```
or
```
git checkout [modified file]
```

## Connections between LD2450 and ESP8266

LD2450		ESP8266
- GND		GND
- 5V		5V
- TX		D7
- RX		D8

## Building and downloading code into an ESP

Use either VS Codium (preferred), VS Code or Arduino IDE to compile and download both code and data into ESP8266.

You may want to change options in platformio.ini (or FF_WebServerCfh.h if using Arduino IDE) to add/reduce functionalities.

If using Arduino IDE, you'll have to install the following libraries:
	- https://github.com/me-no-dev/ESPAsyncTCP
	- https://github.com/gmag11/NtpClient
	- https://github.com/me-no-dev/ESPAsyncWebServer
	- https://github.com/bblanchon/ArduinoJson
	- https://github.com/arcao/Syslog
	- https://github.com/marvinroger/async-mqtt-client
	- https://github.com/joaolopesf/RemoteDebug (version 2.1.2)
	- https://github.com/FlyingDomotic/FF_Trace
	- https://github.com/FlyingDomotic/FF_Interval
	- https://github.com/FlyingDomotic/FF_WebServer