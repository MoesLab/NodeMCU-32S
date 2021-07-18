/*********
Moe Azizi, March 2021
NodeMCU-32S a collection of advanced project packaged in one project
include Async Web Server, SD Card functions 
drived from main origin.cpp
fix needed for 
1: FILENAME ->name() to work

*********/

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Arduino_JSON.h>
#include <Adafruit_NeoPixel.h>

#include "SPIFFS.h"
#include "SD.h"
#include "FS.h"
#include "SPI.h"
#include "FastLED.h"

// Use if you want to force the software SPI subsystem to be used for some reason (generally, you don't)
// #define FASTLED_FORCE_SOFTWARE_SPI
// Use if you want to force non-accelerated pin access (hint: you really don't, it breaks lots of things)
// #define FASTLED_FORCE_SOFTWARE_SPI
// #define FASTLED_FORCE_SOFTWARE_PINS

///////////////////////////////////////////////////////////////////////////////////////////
//
// Move a white dot along the strip of leds.  This program simply shows how to configure the leds,
// and then how to turn a single pixel white and then off, moving down the line of pixels.
// 

#define NUM_LEDS 139

#define DATA_PIN 15

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

// This function sets up the ledsand tells the controller about them
void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
   	delay(2000);

    FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS); //right one for 144 strips

}

void loop() {
   // Move a single white led 
   for(int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
      leds[whiteLed] = CRGB::Red;
      FastLED.show();
      delay(2);
      leds[whiteLed] = CRGB::Black;
   }
   //return of the color
    for(int whiteLed = NUM_LEDS; whiteLed >=-1 ; whiteLed = whiteLed - 1) {
      leds[whiteLed] = CRGB::Red;
      FastLED.show();
      delay(2);
      leds[whiteLed] = CRGB::Black;
   }
   delay(2000);
      for(int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
      leds[whiteLed] = CRGB::Green;
      FastLED.show();
      delay(2);
      leds[whiteLed] = CRGB::Black;
   }
   //return of the color
    for(int whiteLed = NUM_LEDS; whiteLed >=-1 ; whiteLed = whiteLed - 1) {
      leds[whiteLed] = CRGB::Green;
      FastLED.show();
      delay(2);
      leds[whiteLed] = CRGB::Black;
   }
   delay(2000);
      for(int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
      leds[whiteLed] = CRGB::Blue;
      FastLED.show();
      delay(2);
      leds[whiteLed] = CRGB::Black;
   }
   //return of the color
    for(int whiteLed = NUM_LEDS; whiteLed >=-1 ; whiteLed = whiteLed - 1) {
      leds[whiteLed] = CRGB::Blue;
      FastLED.show();
      delay(2);
      leds[whiteLed] = CRGB::Black;
   }
   delay(2000);
      for(int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
      leds[whiteLed] = CRGB::White;
      FastLED.show();
      delay(2);
      leds[whiteLed] = CRGB::Black;
   }
   //return of the color
    for(int whiteLed = NUM_LEDS; whiteLed >=-1 ; whiteLed = whiteLed - 1) {
      leds[whiteLed] = CRGB::White;
      FastLED.show();
      delay(2);
      leds[whiteLed] = CRGB::Black;
   }
   delay(2000);
}