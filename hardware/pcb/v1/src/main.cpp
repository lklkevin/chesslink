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
StickerReader readerA(PHOTO_PIN_A, ON_LED_A, RED_PIN_IO, GREEN_PIN_IO, BLUE_PIN_IO, IR_PIN, HALL_PIN_A);
StickerReader readerB(PHOTO_PIN_B, ON_LED_B, RED_PIN_IO, GREEN_PIN_IO, BLUE_PIN_IO, IR_PIN, HALL_PIN_B);
StickerReader readerC(PHOTO_PIN_C, ON_LED_C, RED_PIN_IO, GREEN_PIN_IO, BLUE_PIN_IO, IR_PIN, HALL_PIN_C);
StickerReader readerD(PHOTO_PIN_D, ON_LED_D, RED_PIN_IO, GREEN_PIN_IO, BLUE_PIN_IO, IR_PIN, HALL_PIN_D);


// Helper function to print detailed information for each square
void printSquareInfo(StickerReader& reader, int* signature, const char* label) {
  Serial.print("  R: "); Serial.print(signature[0]);
  Serial.print(", G: "); Serial.print(signature[1]);
  Serial.print(", B: "); Serial.print(signature[2]);
  Serial.print(" => Detected: ");
  Serial.print(label);
  
  Serial.print(" | Hall Value: ");
  Serial.print(reader.getHallValue());
  Serial.print(" | Piece Type: ");
  switch (reader.getPieceType()) {
    case PIECE_WHITE: Serial.println("WHITE"); break;
    case PIECE_BLACK: Serial.println("BLACK"); break;
    default: Serial.println("NONE"); break;
  }
  
  Serial.print("  IR Value: ");
  Serial.print(reader.getIRValue());
  Serial.print(" | Piece Present: ");
  Serial.println(reader.isPiecePresent() ? "YES" : "NO");
}


void setup() {
  Serial.begin(9600);
  
  // Initialize all readers
  readerA.begin();
  readerB.begin();
  readerC.begin();
  readerD.begin();
  
  Serial.println("All Sticker Readers Initialized");
}

void loop() {
  // Variables to track which sensors have changed
  bool changedA = readerA.hasAmbientChanged();
  bool changedB = readerB.hasAmbientChanged();
  bool changedC = readerC.hasAmbientChanged();
  bool changedD = readerD.hasAmbientChanged();
  
  bool ambientChanged = changedA || changedB || changedC || changedD;
  
  // Static variables to store previous readings across loop calls
  static int signatureA[4] = {0}, signatureB[4] = {0}, signatureC[4] = {0}, signatureD[4] = {0};
  static const char* labelA = "Empty", *labelB = "Empty", *labelC = "Empty", *labelD = "Empty";
  
  if (ambientChanged) {
    Serial.println("Ambient light changed, checking changed pieces...");
    
    // Only read signatures for squares that have changed
    if (changedA) {
      readerA.readSignature(signatureA);
      labelA = readerA.identifySticker();
      Serial.println("Square A changed!");
    }
    
    if (changedB) {
      readerB.readSignature(signatureB);
      labelB = readerB.identifySticker();
      Serial.println("Square B changed!");
    }
    
    if (changedC) {
      readerC.readSignature(signatureC);
      labelC = readerC.identifySticker();
      Serial.println("Square C changed!");
    }
    
    if (changedD) {
      readerD.readSignature(signatureD);
      labelD = readerD.identifySticker();
      Serial.println("Square D changed!");
    }
    
    // Get FEN characters for each square (using cached values for unchanged squares)
    const char* fenA = readerA.getFENFromLabel(labelA);
    const char* fenB = readerB.getFENFromLabel(labelB);
    const char* fenC = readerC.getFENFromLabel(labelC);
    const char* fenD = readerD.getFENFromLabel(labelD);
    
    // Print combined FEN
    Serial.print("Combined FEN: ");
    Serial.print(fenA);
    Serial.print(fenB);
    Serial.print(fenC);
    Serial.println(fenD);
    
    // Print detailed information only for changed squares
    if (changedA) {
      Serial.println("Square A:");
      printSquareInfo(readerA, signatureA, labelA);
    }
    
    if (changedB) {
      Serial.println("Square B:");
      printSquareInfo(readerB, signatureB, labelB);
    }
    
    if (changedC) {
      Serial.println("Square C:");
      printSquareInfo(readerC, signatureC, labelC);
    }
    
    if (changedD) {
      Serial.println("Square D:");
      printSquareInfo(readerD, signatureD, labelD);
    }
    
    Serial.println("=====================================");
  } else {
    // If no change, just do a quick check every 200ms
    delay(200);
  }
}
