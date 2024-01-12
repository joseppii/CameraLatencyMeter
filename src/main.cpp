// SPDX-FileCopyrightText: 2022 Limor Fried for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <Arduino.h>
#include "Adafruit_MAX1704X.h"
#include <Adafruit_NeoPixel.h>
#include "Adafruit_TestBed.h"

#include <Adafruit_ST7789.h> 
#include <Fonts/FreeSans12pt7b.h>
#include "Adafruit_TSL2591.h"

extern Adafruit_TestBed TB;

Adafruit_MAX17048 lipo;
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

GFXcanvas16 canvas(240, 135);
const int photoresistorPin = A0; // Analog pin for the photoresistor
const int ledPin = 5;           // Digital pin for an LED
const int interruptPin = 2;     // Digital pin for interrupt

volatile bool ledOn = false;
volatile unsigned long ledOnTime = 0;
volatile unsigned long photoresistorTime = 0;

const int RESOLUTION = 12;

void ledTurnedOn() {
  ledOnTime = micros(); // Capture the precise time when the LED turns on
  ledOn = true;
}

void setup() {
  Serial.begin(115200);
  //while (! Serial) delay(10);
  analogReadResolution(RESOLUTION);
  pinMode(photoresistorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(0, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), ledTurnedOn, RISING);
  delay(100);
  
  TB.neopixelPin = PIN_NEOPIXEL;
  TB.neopixelNum = 1; 
  TB.begin();
  TB.setColor(WHITE);

  display.init(135, 240);           // Init ST7789 240x135
  display.setRotation(3);
  canvas.setFont(&FreeSans12pt7b);
  canvas.setTextColor(ST77XX_WHITE); 

  if (!lipo.begin()) {
    Serial.println(F("Couldnt find Adafruit MAX17048?\nMake sure a battery is plugged in!"));
    while (1) delay(10);
  }
    
  Serial.print(F("Found MAX17048"));
  Serial.print(F(" with Chip ID: 0x")); 
  Serial.println(lipo.getChipID(), HEX);

  pinMode(2, INPUT_PULLDOWN);

}

uint8_t j = 0;

void loop() {

Serial.println("Press the button to start the measurement...");
  while (digitalRead(interruptPin) == LOW) {
    delay(10);
  }
  int photo= analogRead(photoresistorPin);
  Serial.print(photo);
// Button pressed, start the measurement
  Serial.println("Button pressed! Measurement started.");

  // Turn on the LED
  digitalWrite(ledPin, HIGH);
  

// Wait for the photoresistor to detect the LED change
  while (ledOn && analogRead(photoresistorPin) > 500) {
    delayMicroseconds(10);
  }
  photoresistorTime = micros();
  // Turn off the LED
  digitalWrite(ledPin, LOW);

  // Calculate and print the time taken
  unsigned long responseTime = photoresistorTime - ledOnTime;
  Serial.print("Photoresistor response time: ");
  Serial.print(responseTime);
  Serial.println(" microseconds");

  ledOn = false;
  ledOnTime = 0;
  photoresistorTime = 0;
  // Wait for a brief moment before allowing another measurement
  delay(500);
  return;
}
