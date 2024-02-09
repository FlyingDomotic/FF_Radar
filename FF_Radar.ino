/*

	This code connects a LD2450 20 GHz radar module to network through an ESP8266.

	It implements on ESP8266 a fully asynchronous Web server,
		with MQTT connection,
		Arduino and Web OTA,
		telnet serial debug, 
		syslog trace, 
		optional external hardware watchdog,
		and Domoticz connectivity.

	It also has a local file system to host user and server files.

	LD2450/ESP8266 connectivity:
		LD2450 5V to ESP 5V,
		LD2450 GND to ESP GND,
		LD2450 RX to ESP D8,
		LD2450 TX to ESP D7.
	
	If used, watchdog output pin is ESP D4. An example of watchdog circuit with an ATtiny85 can be found at https://github.com/FlyingDomotic/FF_Watchdog
	
	Code synchronizes radar detection with a Domoticz presence detector, if needed. It could also present a movment switch.
		Presence indicates a person in sensor range, even not moving, while movment indicates a target move.

	If can also send periodically a JSON message with count of target detected (up to 3), with X, Y, speed and resolution for each target.
	
	Same message can also be returned on (http) request.

	This code is based on FF_WebServer (https://github.com/FlyingDomotic/FF_WebServer).

	Written and maintained by Flying Domotic (https://github.com/FlyingDomotic/FF_Radar).

*/

#define VERSION "1.0.0"										// Version of this code
#include <FF_WebServer.h>									// Defines associated to FF_WebServer class
#include <FF_HLK_LD2450.h>									// Library for LD2450 radar

//	User internal data
//		Declare here user's data needed by user's code

FF_HLK_LD2450 ld2450;
int16_t targetX[3];
int16_t targetY[3];
int16_t targetSpd[3];
uint16_t targetRes[3];
uint8_t targetCount = 0;
uint8_t targetMoving = 0;
uint8_t oldTargetCount = 255;
uint8_t oldTargetMoving = 255;
bool lastReadStatus;

//	User configuration data (should be in line with userconfigui.json)
//		Declare here user's configuration data needed by user's code
int configMQTT_Interval = 0;								// Interval (in second) to send MQTT updates
int presenceFlagIdx = 0;									// Domoticz's presence flag IDX (on/off)
int movingFlagIdx = 0;										// Domoticz's move flag IDX (on/off target moving)
int presenceCountIdx = 0;									// Domoticz's presence count IDX (# target detected)
int movingCountIdx = 0;										// Domoticz's move count IDX (# moving target)
unsigned long lastMqttTime = 0;								// Time of last MQTT sent message

//	User routines
void ld2450Loop() {
	// Update LD2450 data
	lastReadStatus = ld2450.refreshRadarData();
	if (lastReadStatus) {
		// Load read data
		targetCount = 0;
		targetMoving = 0;
		for (int i=0; i<3; i++) {
			if (targetX[i] != ld2450.getTargetX(i) || targetY[i] != ld2450.getTargetY(i)) {
				targetMoving++;
			}
			targetX[i] = ld2450.getTargetX(i);
			targetY[i] = ld2450.getTargetY(i);
			targetSpd[i] = ld2450.getTargetSpeed(i);
			targetRes[i] = ld2450.getTargetResolution(i);
			if (targetX[i] != 0 and targetY[i] != 0) {
				targetCount++; 
			}
		}
		// Update Domoticz presence flag and count if needed
		if (oldTargetCount != targetCount) {
			if (presenceCountIdx) {
				char tempBuffer[10];
				snprintf_P(tempBuffer, sizeof(tempBuffer),PSTR("%d"), targetCount);
				FF_WebServer.sendDomoticzValues(presenceCountIdx, tempBuffer, 0);
			}
			if (presenceFlagIdx) {
				if ((oldTargetCount != 0 && targetCount == 0) || (oldTargetCount == 0  && targetCount != 0)) {
					FF_WebServer.sendDomoticzSwitch(presenceFlagIdx, targetCount != 0);
				}
			}
			oldTargetCount = targetCount;
		}

		// Update Domoticz target moving flag and count if needed
		if (oldTargetMoving != targetMoving) {
			if (movingCountIdx) {
				char tempBuffer[10];
				snprintf_P(tempBuffer, sizeof(tempBuffer),PSTR("%d"), targetMoving);
				FF_WebServer.sendDomoticzValues(movingCountIdx, tempBuffer, 0);
			}
			if (movingFlagIdx) {
				if ((oldTargetMoving != 0 && targetMoving == 0) || (oldTargetMoving == 0  && targetMoving != 0)) {
					FF_WebServer.sendDomoticzSwitch(movingFlagIdx, targetMoving != 0);
				}
			}
			oldTargetMoving = targetMoving;
		}
	}
	// Send MQTT status if needed
	if ((millis() - lastMqttTime) >= (((unsigned long) configMQTT_Interval) * 1000) && configMQTT_Interval) {
		lastMqttTime = millis();
		char tempBuffer[500];
		tempBuffer[0] = 0;

		snprintf_P(tempBuffer, sizeof(tempBuffer),
			PSTR("{"
					"\"targetCount\": %d,"
					"\"targetMoving\": %d,"
					"\"target1\": {\"x\":%d, \"y\":%d, \"speed\":%d, \"resolution\":%d},"
					"\"target2\": {\"x\":%d, \"y\":%d, \"speed\":%d, \"resolution\":%d},"
					"\"target3\": {\"x\":%d, \"y\":%d, \"speed\":%d, \"resolution\":%d}"
				"}")
			,targetCount
			,targetMoving
			,targetX[0]
			,targetY[0]
			,targetSpd[0]
			,targetRes[0]
			,targetX[1]
			,targetY[1]
			,targetSpd[1]
			,targetRes[1]
			,targetX[2]
			,targetY[2]
			,targetSpd[2]
			,targetRes[2]
		);
		FF_WebServer.mqttPublish("value", tempBuffer, true);
	}
}

// Declare here used callbacks
static CONFIG_CHANGED_CALLBACK(onConfigChangedCallback);
static DEBUG_COMMAND_CALLBACK(onDebugCommandCallback);
static REST_COMMAND_CALLBACK(onRestCommandCallback);
static JSON_COMMAND_CALLBACK(onJsonCommandCallback);

// Here are the callbacks code

/*!

	This routine is called when permanent configuration data has been changed.
		User should call FF_WebServer.load_user_config to get values defined in userconfigui.json.
		Values in config.json may also be get here.

	\param	none
	\return	none

*/

CONFIG_CHANGED_CALLBACK(onConfigChangedCallback) {
	trace_info_P("Entering %s", __func__);
	FF_WebServer.load_user_config("MQTTInterval", configMQTT_Interval);
	FF_WebServer.load_user_config("presenceFlagIdx", presenceFlagIdx);
	FF_WebServer.load_user_config("presenceCountIdx", presenceCountIdx);
	FF_WebServer.load_user_config("movingFlagIdx", movingFlagIdx);
	FF_WebServer.load_user_config("movingCountIdx", movingCountIdx);
}

/*!

	This routine is called when a user's debug command is received.

	User should analyze here debug command and execute them properly.

	\note	Note that standard commands are already taken in account by server and never passed here.

	\param[in]	debugCommand: last debug command entered by user
	\return	none

*/

DEBUG_COMMAND_CALLBACK(onDebugCommandCallback) {
	trace_info_P("Entering %s", __func__);
	// "user" command is a standard one used to print user variables
	if (debugCommand == "user") {
		trace_info_P("traceFlag=%d", FF_WebServer.traceFlag);
		trace_info_P("debugFlag=%d", FF_WebServer.debugFlag);
		// -- Add here your own user variables to print
		trace_info_P("configMQTT_Interval=%d", configMQTT_Interval);
		trace_info_P("presenceFlagIdx=%d", presenceFlagIdx);
		trace_info_P("presenceCountIdx=%d", presenceCountIdx);
		trace_info_P("movingFlagIdx=%d", movingFlagIdx);
		trace_info_P("movingCountIdx=%d", movingCountIdx);
		trace_info_P("lastReadStatus=%d", lastReadStatus);
		trace_info_P("targetCount=%d", targetCount);
		trace_info_P("targetMoving=%d", targetMoving);
		for (int i=0; i<3; i++) {
			trace_info_P("targetX[%d]=%d", i, targetX[i]);
			trace_info_P("targetY[%d]=%d", i, targetY[i]);
			trace_info_P("targetSpd[%d]=%d", i, targetSpd[i]);
			trace_info_P("targetRes[%d]=%d", i, targetRes[i]);
		}
		// -----------
		return true;
	// Put here your own debug commands
	// -----------
	}
	return false;
}

/*!

	This routine analyze and execute REST commands sent through /rest GET command
	It should answer valid requests using a request->send(<error code>, <content type>, <content>) and returning true.

	If no valid command can be found, should return false, to let server returning an error message.

	\note	Note that minimal implementation should support at least /rest/values, which is requested by index.html
		to get list of values to display on root main page. This should at least contain "header" topic,
		displayed at top of page. Standard header contains device name, versions of user code, FF_WebServer template
		followed by device uptime. You may send something different.
		It should then contain user's values to be displayed by index_user.html file.

	\param[in]	request: AsyncWebServerRequest structure describing user's request
	\return	true for valid answered by request->send command, false else

*/
REST_COMMAND_CALLBACK(onRestCommandCallback) {
	if (request->url() == "/rest/values") {
		char tempBuffer[500];
		tempBuffer[0] = 0;

		snprintf_P(tempBuffer, sizeof(tempBuffer),
			PSTR(
				// -- Put header composition
				"header|%s V%s/%s, up since %s|div\n"
				// -- Put here user variables to be available in index_user.html
				"tCnt|%d|div\n"
				"tMov|%d|div\n"
				"tX1|%d|div\n"
				"tY1|%d|div\n"
				"tSpd1|%d|div\n"
				"tRes1|%d|div\n"
				"tX2|%d|div\n"
				"tY2|%d|div\n"
				"tSpd2|%d|div\n"
				"tRes2|%d|div\n"
				"tX3|%d|div\n"
				"tY3|%d|div\n"
				"tSpd3|%d|div\n"
				"tRes3|%d|div\n"
				// -----------------
				)
			// -- Put here values of header line
			,FF_WebServer.getDeviceName().c_str()
			,VERSION, FF_WebServer.getWebServerVersion()
			,NTP.getUptimeString().c_str()
			// -- Put here values in index_user.html
			,targetCount
			,targetMoving
			,targetX[0]
			,targetY[0]
			,targetSpd[0]
			,targetRes[0]
			,targetX[1]
			,targetY[1]
			,targetSpd[1]
			,targetRes[1]
			,targetX[2]
			,targetY[2]
			,targetSpd[2]
			,targetRes[2]
			// -----------------
			);
		request->send(200, "text/plain", tempBuffer);
		return true;
	}

	trace_info_P("Entering %s", __func__);					// Put trace here as /rest/value is called every second0
	if (request->url() == "/rest/err400") {
		request->send(400, "text/plain", "Error 400 requested by user");
		return true;
	}
	return false;
}

/*!

	This routine analyze and execute JSON commands sent through /json GET command
		It should answer valid requests using a request->send(<error code>, <content type>, <JSON content>)
		and returning true.

		If no valid command can be found, should return false, to let template code returning an error message.

	\param[in]	request: AsyncWebServerRequest structure describing user's request
	\return	true for valid answered by request->send command, false else

*/

JSON_COMMAND_CALLBACK(onJsonCommandCallback) {
	trace_info_P("Entering %s", __func__);
	if (request->url() == "/json/values") {
		char tempBuffer[500];
		tempBuffer[0] = 0;

		snprintf_P(tempBuffer, sizeof(tempBuffer),
			PSTR("{"
					"\"targetCount\": %d,"
					"\"targetMoving\": %d,"
					"\"target1\": {\"x\":%d, \"y\":%d, \"speed\":%d, \"resolution\":%d},"
					"\"target2\": {\"x\":%d, \"y\":%d, \"speed\":%d, \"resolution\":%d},"
					"\"target3\": {\"x\":%d, \"y\":%d, \"speed\":%d, \"resolution\":%d}"
				"}")
			,targetCount
			,targetMoving
			,targetX[0]
			,targetY[0]
			,targetSpd[0]
			,targetRes[0]
			,targetX[1]
			,targetY[1]
			,targetSpd[1]
			,targetRes[1]
			,targetX[2]
			,targetY[2]
			,targetSpd[2]
			,targetRes[2]
		);
		request->send(200, "text/json", tempBuffer);
		return true;
	}

	return false;
}

//	This is the setup routine.
//		Initialize Serial, LittleFS and FF_WebServer.
//		You also have to set callbacks you need,
//		and define additional debug commands, if required.
void setup() {
	// Open serial connection
	Serial.begin(74880);
	// Start Little File System
	LittleFS.begin();
	// Start FF_WebServer
	FF_WebServer.begin(&LittleFS, VERSION);
	// Set user's callbacks
	FF_WebServer.setConfigChangedCallback(&onConfigChangedCallback);
	FF_WebServer.setDebugCommandCallback(&onDebugCommandCallback);
	FF_WebServer.setRestCommandCallback(&onRestCommandCallback);
	FF_WebServer.setJsonCommandCallback(&onJsonCommandCallback);

	// Set Serial to LD speed (256000 bds)
	ld2450.begin(256000);
	// Swap Serial to D7/D8
	Serial.swap();
}

//	This is the main loop.
//	Do what ever you want and call FF_WebServer.handle()
void loop() {
	// User part of loop
  	ld2450Loop();
	// Manage Web Server
	FF_WebServer.handle();
}