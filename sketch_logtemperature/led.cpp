#include "led.h"

#include <arduino.h>

// Onboard LED I/O pin
const int PIN_LED = 2;      // Controls the onboard LED

// LED Flashing constants
const unsigned int BLINK_LENGTH=150;              // A "standard" blink
const unsigned int BLINK_LENGTH_HEARTBEAT=10;     // A really quick flash to show that the program is running


// Standard constructor
LedClass::LedClass() {
  // initialize the LED digital pin as an output.
  pinMode(PIN_LED, OUTPUT);
}


// Turns the LED on/off
void LedClass::light(bool turnOn) {
  int state = turnOn ? LOW : HIGH;    // turn the LED on by making the voltage LOW  
  digitalWrite(PIN_LED, state);
}

// Turns the LED on for the specified time (milliseconds) and then turns it off
// Note: This function blocks - so keep ms pretty small!
void LedClass::blinkMillis(int ms) {
  light(true);
  delay(ms);
  light(false);
  delay(ms);
}

// Blinks the LED once
void LedClass::blink() {  
  blinkMillis(BLINK_LENGTH);
}

// Blinks the LED to signify success
void LedClass::blinkSuccess() {
  for (int i=0; i<1; i++) {
    blink();
  }
}

// Blinks the LED to signify failure
void LedClass::blinkFailure() {
  for (int i=0; i<6; i++) {
    blink();
  } 
}

// Blinks the LED really quickly to signify that the program is running...
void LedClass::blinkHeartbeat() {
  Led.blinkMillis(BLINK_LENGTH_HEARTBEAT);
}


// Define the singleton instance
LedClass Led;

