/*
 * config.h
 * 
 * Sample config header file
 * Contains various configuration settings.
 * 
 * 
 */


// WiFi connection properties
const char* WIFI_SSID = "TODO-YOUR-SSID";// your wifi network SSID (name) 
const char* WIFI_PASSWORD = "TODO-YOUR-PASSWORD";    // your wifi network password (use for WPA, or use as key for WEP)

// The server where we save the temperature readings
const char* UBIDOTS_SERVER = "app.ubidots.com";

// UBIDOTS auth token
const char* UBIDOTS_TOKEN = "TODO-YOUR-TOKEN"; //Get this from ubidots to identify you after creating an account there

// The relative url to add/access the saved temperature readings 
const char* UBIDOTS_URL_VALUES = "/api/v1.6/variables/TODO-YOUR-KEY/values"; //Get the Key from ubidots to associate this data with a particular ubidots variable


// Logging level
const boolean LOG_DEBUG = false;
const boolean LOG_INFO  = true || LOG_DEBUG;
const boolean LOG_ERROR  = true || LOG_INFO;


// GPIO pin which DS18B20 is plugged into. Port 2 on the NodeMCU is pin D4
#define ONE_WIRE_BUS 2

// Polling interval (time in ms between temperature readings)
const unsigned long POLLING_INTERVAL = 2000; //Can not be less than 1 second as sensors take about 750ms to complete the reading requested after each poll.
const unsigned long MAXIMUM_REPORTING_INTERVAL = 600000; //milliseconds

// Maximum number of retries when trying to connect to the WiFi access point when sending an HTTP request
const unsigned int MAX_RETRIES_WIFI_CONNECT = 1; 

// Maximum number of retries when sending temperature samples to the server (set to 0 to give up after the first attempt)
const unsigned int MAX_RETRIES_SAMPLE = 5; 

// Temperature difference to trigger logging new data to the server
const float TEMPERATURE_TRIGGER = 0.25;

