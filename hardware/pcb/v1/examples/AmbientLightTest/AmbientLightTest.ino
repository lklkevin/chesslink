#include <Arduino.h>
#include "StickerReader.h"
#include "LEDUtils.h"

const int PHOTO_PIN_A = A1;
const int PHOTO_PIN_B = A2;
const int PHOTO_PIN_C = A0;
const int PHOTO_PIN_D = A3;

const int HALL_PIN_A = A6;
const int HALL_PIN_B = A5;
const int HALL_PIN_C = A7;
const int HALL_PIN_D = A4;

const int RED_PIN_IO = 6;
const int GREEN_PIN_IO = 3;
const int BLUE_PIN_IO = 5;
const int IR_PIN = 2;

const int ON_LED_A = 10;
const int ON_LED_B = 9; 
const int ON_LED_C = 11;
const int ON_LED_D = 12;

// Create readers for all four squares

// Helper function to print detailed information for each square

void setup() {
  Serial.begin(9600);
  
  // Initialize all readers
  pinMode(PHOTO_PIN_A, INPUT);
}

void loop() {
  Serial.println(analogRead(PHOTO_PIN_A));
}
