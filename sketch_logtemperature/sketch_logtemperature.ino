
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
// Include the libraries needed for DS18B20 temperature measurement from https://github.com/milesburton/Arduino-Temperature-Control-Library
#include <DallasTemperature.h> 
/*
  LogTemperature
andalasa
  Logs changes to temperature to a ubidots server.
  For use with an ESP8266 board (ESP-12E)

 */

// Note: These include files are located in: C:\Users\<USER>\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.0.0\libraries\ESP8266WiFi\src
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <OneWire.h> // Used for talking to DS18B20 temp sensor. Be careful to use the esp8266 version, usually there will be 2 versions installed in the IDE

// Various configurations, including authentication details...
#include "config.h"

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress Thermometer;


// Some utilities for operating the onboard LED
#include "led.h"


// Function prototypes...
// Note: By rights these should not be needed - but are necessary when including ESP8266WiFi.h !
boolean connectToWiFi(int maxRetries);



//-------------------------------------------------------------------------------------------------------------------


/*
 * Issues an HTTP GET or POST request and returns the response
 * 
 * server: (eg. www.google.com)
 * url: Relative url including query string (eg. /search?q=arduino)
 * postData: The post data (for POST requests)
 * method: GET or POST (if NULL, defaults to POST if postData is not null, and GET otherwise)
 * port: Generally 80 for HTTP and 443 for HTTPS
 *
 */
String issueHttpRequest(const char* server, const char* url, const char* postData=NULL, const char* httpMethod=NULL, int port=80) {

  // First, ensure we have a WI-FI connection
  if (!connectToWiFi(MAX_RETRIES_WIFI_CONNECT)) {
    // We cannot connect to the WiFi access point - so simply return an empty response (which is what we do when the HTTP request fails)
    return "";
  }

  if (!httpMethod) {
    // Determine the GET/POST method based on availability of postData
    httpMethod = postData ? "POST" : "GET";
  }
  
  // Determine if this is a POST request...
  boolean isPost = strcmp(httpMethod, "POST") == 0;

  if (LOG_DEBUG) Serial.println("\nConnecting to server...");


  // Open a connection to the server
  WiFiClient client;
  boolean ok = client.connect(server, port);
  if (ok) {
    
    if (LOG_DEBUG) Serial.println("connected to server");
  

    String header = "";
    header = header + httpMethod + " " + url + " HTTP/1.1\n";
    header = header + "Host: " + server + "\n";
    header = header + "Connection: close\n";
    if (isPost) {
      header = header + "Content-Type: application/x-www-form-urlencoded\n";
      header = header + "Content-Length: " + strlen(postData) + "\n";
    }

    if (LOG_DEBUG) {
      Serial.printf("header = %s\n", header.c_str());
      Serial.printf("postData = %s\n", postData ? postData : "<NULL>");
    }
     
    // Issue the HTTP request:
    client.println(header);

    if (postData) {
      client.print(postData);
    }
    
  } else {
    // Hmmm - we could not connect to the server!
    if (LOG_ERROR) Serial.printf("Failed to connect to server: %s port=%d\n", server, port);
  }

  // Get the response
  String response = "";

  if (ok) {
    if (LOG_DEBUG) Serial.println("Reading response...");
     
    while (client.connected()) {
  
      // Read the response...
      while (client.available()) {
        char c = client.read();
        // TODO: This is pretty lame - we should probably write into a temp buffer
        response += c;
        //Serial.write(c);
      }
  
      if (client.connected()) {
        if (LOG_DEBUG && response.length() > 0) {
          Serial.printf("Response so far:\n%s\n", response.c_str());
          Serial.println("Waiting for more response...");
        }
        delay(1000);
      }
    }
  }
  
  if (LOG_DEBUG) Serial.println("\ndisconnecting from server.");
  client.stop();

  return response;
}


// Sends a new temperature value to the server
boolean sendTemperatureValue(float value) {
  
  // Note: The authentication token must be included as a query string in the request, and not in the posted parameters!!!
  String url = UBIDOTS_URL_VALUES;
  url = url + "?token=" + UBIDOTS_TOKEN;

  // Convert the temperature value to a string with 2 decimal places
  char sValue[20];
  //ftoa(sValue, value, 2);
  dtostrf(value, 3, 2, sValue);

  
  String postData = "";
  postData = postData + "value=" + sValue;

  String response = issueHttpRequest(UBIDOTS_SERVER, url.c_str(), postData.c_str());

  // Ensure the response looks like it succeeded...
  // This is pretty lame, but we expect to see a "201 CREATED" in the response...
  boolean ok = response.indexOf("201 CREATED") > 0;

  if (ok) {
    if (LOG_INFO) Serial.printf("Sent temperature: %s\n", sValue);    
    Led.blinkSuccess();
  } else {
    if (LOG_ERROR) {
      Serial.printf("Failed to send temperature data!\n");
      Serial.printf("Request: %s\n", url.c_str());
      Serial.printf("PostData: %s\n", postData.c_str());
      Serial.printf("Response: %s\n", response.c_str());
    }
    Led.blinkFailure();
  }


  // For testing... Issue a GET request...
  //String response2 = issueHttpRequest(UBIDOTS_SERVER, url.c_str());
  //Serial.printf("GET REQUEST: \n%s\n", response2.c_str());
  return ok;
}


// Function to delete all the values from the server...
// Maybe we can jumper some pins to allow the board to delete values when required???
void deleteAllValues() {
  // Note: The authentication token must be included as a query string in the request, and not in the posted parameters!!!
  String url = UBIDOTS_URL_VALUES;
  url = url + "/0/9999999999000";     // Deletes all values between the 2 timestamps...
  url = url + "?token=" + UBIDOTS_TOKEN;
  
  String response = issueHttpRequest(UBIDOTS_SERVER, url.c_str(), NULL, "DELETE");
  Serial.printf("DELETE Response: %s\n", response.c_str());

}



void printWifiStatus() {
   // print the SSID of the network you're attached to:
   Serial.printf("SSID: %s\n", WiFi.SSID().c_str());

   // print your WiFi shield's IP address:
   IPAddress ip = WiFi.localIP();
   Serial.print("IP Address: ");
   Serial.println(ip);

   // print the received signal strength:
   long rssi = WiFi.RSSI();
   Serial.printf("signal strength (RSSI): %ld dBm\n", rssi);
   //Serial.print(rssi);
   //Serial.println(" dBm");
}


// Returns true if we are currently connected to the wi-fi access point
boolean isConnectedToWiFi() {
  // See if we are currently connected to the WI-FI
  int status = WiFi.status();
  return status == WL_CONNECTED;
}

/*
 * Connects to the wi-fi access point
 * maxRetries is the maximum number of attempts that we want to make. Set to -1 to loop forever until connected (which is the default)
 * Returns true if connected, false otherwise
 */
boolean connectToWiFi(int maxRetries = -1) {
  
  // TODO: Must test for disconnection from the router...

  // See if we are currently connected to the WI-FI
  boolean isConnected = isConnectedToWiFi();
  boolean wasConnected = isConnected;

  int maxAttempts = maxRetries + 1;
  int attemptCount = 0;
  while (!isConnected && (attemptCount++<maxAttempts || maxRetries<0)) {
    
    // Try to connect...
    if (LOG_INFO) Serial.printf("Attempting to connect to Wifi - SSID: %s (Attempt #%d)\n", WIFI_SSID, attemptCount);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    int status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
    if (LOG_DEBUG) Serial.printf("Status=%d\n", status);

    if (status != WL_CONNECTED) {
      // wait 10 seconds for connection:
      Led.blinkFailure();
      delay(10000);
      
    } else if (!isConnectedToWiFi()) {
      // Hmmm. This should never happen
      if (LOG_ERROR) Serial.println("**** UNKNOWN ERROR: WiFi.begin returned WL_CONNECTED but isConnectedToWiFi=False!!!!");
      Led.blinkFailure();
      Led.blinkFailure();
      Led.blinkFailure();
      Led.blinkFailure();
      delay(10000);
      
    } else {
      // We have connected
      isConnected = true;
      
      if (LOG_INFO) {
        Serial.println("Connected to wifi");
        printWifiStatus();
      }
      Led.blinkSuccess();
    }
  }

  if (LOG_INFO && !isConnected) {
    Serial.println("Failed to connect to wifi");
  }
  
  return isConnected;
}


// Emulates a temperature sensor - for testing the code without a physical sensor connected 
int analogReadDummy(int pin) {
  static int sensorVal = 150;   // Start off at around 0 degrees

  // The sensor appears to go from -50deg to 280deg
  // Therefore each sensor unit is about 0.33 degrees (or 1 degree per 3 sensor units)

  // Generate a random number between -2 and 4 inclusive
  int val = random(0,7) - 2;

  sensorVal += val;

  // Ensure the sensor does not overflow...
  if (sensorVal > 300 || sensorVal < 100) sensorVal = 150;

  return sensorVal;
}


/*
 * This is the main function that reads all sensors, examines the data and logs info to the server if required
 */
void processAllSensors() {
  static float lastTemperatureSample = -999;
  static long TimeSinceLast = 0;
  // TODO: Handle multiple sensors...
  TimeSinceLast = TimeSinceLast + POLLING_INTERVAL;
  float TemperatureSample = sensors.getTempCByIndex(0); 
  sensors.requestTemperatures();   // Send the command to get temperatures. POLLING_INTERVAL should be at least 1 second to allow this to complete before we get here again.
  if (LOG_DEBUG) {
    // Note: Serial.printf does not support floating point formats (%f)
    //Serial.printf("Temp Sample = %3.2f  Last=%3.2f\n", TemperatureSample, lastTemperatureSample);
    Serial.print("Temp sample = ");
    Serial.print(TemperatureSample);
    Serial.print("  Last = ");
    Serial.println(lastTemperatureSample);
  }
  float temperatureDiff = abs(TemperatureSample - lastTemperatureSample);
  if (((temperatureDiff > TEMPERATURE_TRIGGER)| (TimeSinceLast > MAXIMUM_REPORTING_INTERVAL))& (TemperatureSample!=-127)) //If no sensor present temperature = -127
  {
    TimeSinceLast = 0;
    // Log the sample to the server...
    int retryCount = 0;
    boolean ok = sendTemperatureValue(TemperatureSample);
    while (!ok && retryCount++ <= MAX_RETRIES_SAMPLE) {
      Serial.println("Request failed. Retrying...");
      ok = sendTemperatureValue(TemperatureSample);
    }

    if (ok) {
      // The data has been saved on the server
      lastTemperatureSample = TemperatureSample;
      
    } else {
      // Abandon this sample as we cannot connect to the server!
      if (LOG_ERROR) Serial.printf("Failed to upload sample after %d attempts\n", retryCount); 
    } 
  }
    else if (LOG_ERROR) Serial.println("No temperature sensor detected");  
}

//============================ Standard setup and loop functions =================================================

// the setup function runs once when you press reset or power the board
void setup() {

  Serial.begin(115200);

  Serial.println("\nInitialising...");

  // Initialise the LED utils
  //setupLed();
  //Led.setup();
  // locate DS18B20 devices on the bus. Internal pullup resistor works but is much larger than than the external 4.7K specified. Risky!
  sensors.begin();
  int SensorCount = sensors.getDeviceCount(); 
  sensors.requestTemperatures(); // Send the command to get temperatures leave as 12 bit by default
 if (LOG_INFO) {
  Serial.print("Found ");
  Serial.print(SensorCount);
  Serial.println(" temperature sensors.");
  for (int i =0; i<SensorCount; i++)
    {
    float temp = sensors.getTempCByIndex(i);
    Serial.print("Temperature ");
    Serial.print(i);
    Serial.print(" ");
    Serial.println(temp);
    }
  }
  sensors.setWaitForConversion(FALSE); //This must be used with caution. See comments on this command in Dallas Temperature.cpp
  /*  sensors.requestTemperatures() will become a non blocking call but it will take
   *  about 750ms for the temp sensor to complete the reading. sensors.getTempCByIndex should not be called until the sensor
   *  has completed taking the reading i.e wait at least 1 second.
   */
  WiFi.mode(WIFI_STA);

  if (LOG_DEBUG) {
    Serial.printf("WiFi.status = %d\n",  WiFi.status());
    // Output some WIFI diagnostics...
    Serial.println("Diags...");
    WiFi.printDiag(Serial );
    }

  // Not really necessary, but let's connect to the wi-fi now...
  // This at least ensures that the board will "error-blink" on startup if the wi-fi is not connected
  // Note that we block here until we are connected to the wi-fi, because if we have no wifi we can't send any data to the server!
  connectToWiFi();

  // Uncomment this to delete all values...
  //deleteAllValues();
   
}


// the loop function runs over and over again forever
void loop() {

  // Blink the LED really quickly to signify that the program is running...
  //Led.blinkHeartbeat();
  
  // Read all sensors and process the data
  processAllSensors();
    
  // Wait a while before getting the next reading
  delay(POLLING_INTERVAL);
}


