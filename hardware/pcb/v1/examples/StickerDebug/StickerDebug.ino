#include <Arduino.h>
#include "StickerReader.h"
// #include "LEDUtils.h"

const int PHOTO_PIN_A = A1;
const int PHOTO_PIN_B = A2;
const int PHOTO_PIN_C = A0;
const int PHOTO_PIN_D = A3;

const int RED_PIN = 6;
const int GREEN_PIN = 3;
const int BLUE_PIN = 5;
const int IR_PIN = 2;

const int ON_LED_A = 10;
const int ON_LED_B = 9; 
const int ON_LED_C = 11;
const int ON_LED_D = 12;

// read only LED A for now
StickerReader reader(PHOTO_PIN_A, ON_LED_A, 
  RED_PIN, GREEN_PIN, BLUE_PIN, IR_PIN);

void setup() {
  Serial.begin(9600);
  reader.begin();
  Serial.println("Sticker Reader Initialized");
}

void loop() {
  int signature[4];
  reader.readSignature(signature);

  Serial.print("R: "); Serial.print(signature[0]);
  Serial.print(", G: "); Serial.print(signature[1]);
  Serial.print(", B: "); Serial.print(signature[2]);
  Serial.print(", IR: "); Serial.print(signature[3]);

  const char* label = reader.identifySticker();
  const char* fen = reader.getFENFromLabel(label);
  Serial.print(" => Detected: ");
  Serial.print(label);
  Serial.print(" | FEN: ");
  Serial.println(fen);

  delay(1000);
}
