/*!

	This file contains user definitions for FF_WebServer.

	It should be adapted by user to configure FF_WebServer for specific needs when using Arduino IDE.
	For PlatformIO, use -D in build_flags section.

*/

#ifndef PLATFORMIO											// Execute these defines only for Arduino IDE
	#define REMOTE_DEBUG									// Enable telnet remote debug (optional)
	//#define SERIAL_DEBUG									// Enable serial debug (optional, only if REMOTE_DEBUG is disabled)
	#define HARDWARE_WATCHDOG_PIN D4						// Enable watchdog external circuit on D4 (optional)
	#define HARDWARE_WATCHDOG_ON_DELAY 5000					// Define watchdog level on delay (in ms)
	#define HARDWARE_WATCHDOG_OFF_DELAY 1					// Define watchdog level off delay (in ms)
	#define HARDWARE_WATCHDOG_INITIAL_STATE 0				// Define watch dog initial state
	#define FF_TRACE_KEEP_ALIVE (5 * 60 * 1000)				// Trace keep alive timer (optional)
	#define FF_TRACE_USE_SYSLOG								// Send trace messages on Syslog (optional)
	#define FF_TRACE_USE_SERIAL								// Send trace messages on Serial (optional)
	#define INCLUDE_DOMOTICZ								// Include Domoticz support (optional)
	#define CONNECTION_LED -1								// Connection LED pin (Built in). -1 to disable
	#define AP_ENABLE_BUTTON -1								// Button pin to enable AP during startup for configuration. -1 to disable
	#define AP_ENABLE_TIMEOUT 240							// (Seconds, max 255) If the device can not connect to WiFi it will switch to AP mode after this time. -1 to disable
	#define DEBUG_FF_WEBSERVER								// Enable internal FF_WebServer debug
	//#define FF_DISABLE_DEFAULT_TRACE						// Disable default trace callback
	//#define NO_SERIAL_COMMAND_CALLBACK					// Disable Serial command callback
#endif