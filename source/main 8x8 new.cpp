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

// _S for Strip on pin 14  and _M for Matrix on pin 15
#define NUM_LEDS_S 139
#define NUM_LEDS_M 64
#define DATA_PIN_S 26 //14
#define DATA_PIN_M 25 //15

#define BRIGHTNESS          255 //32
#define FRAMES_PER_SECOND  120

// This is an array of leds.  One item for each led in your strip.
CRGB leds_S[NUM_LEDS_S];
CRGB leds_M[NUM_LEDS_M];
// This function sets up the ledsand tells the controller about them
void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
   	delay(2000);

    FastLED.addLeds<WS2811, DATA_PIN_S, GRB>(leds_S, NUM_LEDS_S); // for 139 Strips
    FastLED.addLeds<WS2811, DATA_PIN_M, GRB>(leds_M, NUM_LEDS_M); // for 8x8 = 139 Matrix
      // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
   //------------------------------------------ Matrix first
   
     // for(int i = 1; i < NUM_LEDS_M + NUM_LEDS_S; i = i + 1) { //Combine Matrix and Strip
     for(int i = 0; i < NUM_LEDS_M; i = i + 1) {
      leds_M[i] = CRGB::Red;
      FastLED.show();
      delay(2);
      leds_M[i] = CRGB::Black;
   }
   //------------------------------------------ Strip second 
   for(int whiteLed = 0; whiteLed < NUM_LEDS_S; whiteLed = whiteLed + 1) {
      leds_S[whiteLed] = CRGB::Red;
      FastLED.show();
      delay(2);
      leds_S[whiteLed] = CRGB::Black;
   }
   for(int i = NUM_LEDS_M - 1; i >= 0; i = i - 1) {
      leds_M[i] = CRGB::Red;
      FastLED.show();
      delay(2);
      leds_M[i] = CRGB::Black;
   }
   //return of the color
    for(int whiteLed = NUM_LEDS_S - 1; whiteLed >=0 ; whiteLed = whiteLed - 1) {
      leds_S[whiteLed] = CRGB::Red;
      FastLED.show();
      delay(2);
      leds_S[whiteLed] = CRGB::Black;
   }
         FastLED.show();
   delay(2000);
   //----------------------------------------------loop ends
     // for(int i = 1; i < NUM_LEDS_M + NUM_LEDS_S; i = i + 1) { //Combine Matrix and Strip
     for(int i = 0; i < NUM_LEDS_M; i = i + 1) {
      leds_M[i] = CRGB::Green;
      FastLED.show();
      delay(2);
      leds_M[i] = CRGB::Black;
   }
   //------------------------------------------ Strip second 
   for(int whiteLed = 0; whiteLed < NUM_LEDS_S; whiteLed = whiteLed + 1) {
      leds_S[whiteLed] = CRGB::Green;
      FastLED.show();
      delay(2);
      leds_S[whiteLed] = CRGB::Black;
   }
   for(int i = NUM_LEDS_M - 1; i >= 0; i = i - 1) {
      leds_M[i] = CRGB::Green;
      FastLED.show();
      delay(2);
      leds_M[i] = CRGB::Black;
   }
   //return of the color
    for(int whiteLed = NUM_LEDS_S - 1; whiteLed >=0 ; whiteLed = whiteLed - 1) {
      leds_S[whiteLed] = CRGB::Green;
      FastLED.show();
      delay(2);
      leds_S[whiteLed] = CRGB::Black;
   }
         FastLED.show();
   delay(2000);
   //----------------------------------------------loop ends
     // for(int i = 1; i < NUM_LEDS_M + NUM_LEDS_S; i = i + 1) { //Combine Matrix and Strip
     for(int i = 0; i < NUM_LEDS_M; i = i + 1) {
      leds_M[i] = CRGB::Blue;
      FastLED.show();
      delay(2);
      leds_M[i] = CRGB::Black;
   }
   //------------------------------------------ Strip second 
   for(int whiteLed = 0; whiteLed < NUM_LEDS_S; whiteLed = whiteLed + 1) {
      leds_S[whiteLed] = CRGB::Blue;
      FastLED.show();
      delay(2);
      leds_S[whiteLed] = CRGB::Black;
   }
   for(int i = NUM_LEDS_M - 1; i >= 0; i = i - 1) {
      leds_M[i] = CRGB::Blue;
      FastLED.show();
      delay(2);
      leds_M[i] = CRGB::Black;
   }
   //return of the color
    for(int whiteLed = NUM_LEDS_S - 1; whiteLed >=0 ; whiteLed = whiteLed - 1) {
      leds_S[whiteLed] = CRGB::Blue;
      FastLED.show();
      delay(2);
      leds_S[whiteLed] = CRGB::Black;
   }
         FastLED.show();
   delay(2000);
   //----------------------------------------------loop ends
     // for(int i = 1; i < NUM_LEDS_M + NUM_LEDS_S; i = i + 1) { //Combine Matrix and Strip
     for(int i = 0; i < NUM_LEDS_M; i = i + 1) {
      leds_M[i] = CRGB::White;
      FastLED.show();
      delay(2);
      leds_M[i] = CRGB::Black;
   }
   //------------------------------------------ Strip second 
   for(int whiteLed = 0; whiteLed < NUM_LEDS_S; whiteLed = whiteLed + 1) {
      leds_S[whiteLed] = CRGB::White;
      FastLED.show();
      delay(2);
      leds_S[whiteLed] = CRGB::Black;
   }
   for(int i = NUM_LEDS_M - 1; i >= 0; i = i - 1) {
      leds_M[i] = CRGB::White;
      FastLED.show();
      delay(2);
      leds_M[i] = CRGB::Black;
   }
   //return of the color
    for(int whiteLed = NUM_LEDS_S - 1; whiteLed >=0 ; whiteLed = whiteLed - 1) {
      leds_S[whiteLed] = CRGB::White;
      FastLED.show();
      delay(2);
      leds_S[whiteLed] = CRGB::Black;
   }
         FastLED.show();
   delay(2000);
   //----------------------------------------------loop ends

}