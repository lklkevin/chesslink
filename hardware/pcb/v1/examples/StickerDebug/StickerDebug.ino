#include <Arduino.h>
#include "StickerReader.h"

const int PHOTO_PIN = A0;
const int RED_PIN = 6;
const int GREEN_PIN = 3;
const int BLUE_PIN = 5;
const int IR_PIN = 2;

StickerReader reader(PHOTO_PIN, RED_PIN, GREEN_PIN, BLUE_PIN, IR_PIN);

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
  Serial.print(" => Detected: ");
  Serial.println(reader.identifySticker());

  delay(1000);
}
