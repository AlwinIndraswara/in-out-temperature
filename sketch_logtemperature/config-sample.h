/*
 * config.h
 * 
 * Sample config header file
 * Contains various configuration settings.
 * 
 * 
 */


// WiFi connection properties
const char* WIFI_SSID = "TODO-YOUR-SSID";           // your wifi network SSID (name) 
const char* WIFI_PASSWORD = "TODO-YOUR-PASSWORD";    // your wifi network password (use for WPA, or use as key for WEP)

// The server where we save the temperature readings
const char* UBIDOTS_SERVER = "app.ubidots.com";

// UBIDOTS auth token
const char* UBIDOTS_TOKEN = "TODO-YOUR-TOKEN";

// The relative url to add/access the saved temperature readings 
const char* UBIDOTS_URL_VALUES = "/api/v1.6/variables/TODO-YOUR-KEY/values";

// Logging level
const boolean LOG_DEBUG = false;
const boolean LOG_INFO  = true || LOG_DEBUG;
const boolean LOG_ERROR  = true || LOG_INFO;


// I/O pins
const int PIN_TEMPERATURE_INSIDE = -1;   // TODO...
const int PIN_TEMPERATURE_OUTSIDE = -2;   // TODO...

// Polling interval (time in ms between temperature readings)
const unsigned long POLLING_INTERVAL = 2000;
const unsigned long MAXIMUM_REPORTING_INTERVAL = 300000;

// Maximum number of retries when trying to connect to the WiFi access point when sending an HTTP request
const unsigned int MAX_RETRIES_WIFI_CONNECT = 1; 

// Maximum number of retries when sending temperature samples to the server (set to 0 to give up after the first attempt)
const unsigned int MAX_RETRIES_SAMPLE = 5; 

// Temperature difference to trigger logging new data to the server
const float TEMPERATURE_TRIGGER = 0.25;



