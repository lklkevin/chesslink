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
  // Variables to track how sensors have changed
  int changeA = readerA.checkAmbientChange();
  int changeB = readerB.checkAmbientChange();
  int changeC = readerC.checkAmbientChange();
  int changeD = readerD.checkAmbientChange();
  
  bool pieceChangedA = changeA != 0;
  bool pieceChangedB = changeB != 0;
  bool pieceChangedC = changeC != 0;
  bool pieceChangedD = changeD != 0;
  
  bool anyPieceChanged = pieceChangedA || pieceChangedB || pieceChangedC || pieceChangedD;
  
  // Static variables to store previous readings across loop calls
  static int signatureA[4] = {0}, signatureB[4] = {0}, signatureC[4] = {0}, signatureD[4] = {0};
  static const char* labelA = "Empty", *labelB = "Empty", *labelC = "Empty", *labelD = "Empty";
  
  if (anyPieceChanged) {
    delay(500); // Debounce delay and allow piece to land

    Serial.println("Piece change detected, updating board state...");
    
    // Handle square A
    if (pieceChangedA) {
      if (changeA > 0) {
        // Piece placed - do full reading
        readerA.readSignature(signatureA);
        labelA = readerA.identifySticker();
        Serial.println("Square A: Piece placed");
      } else {
        // Piece removed - mark as empty without LED flash
        labelA = "Empty";
        Serial.println("Square A: Piece removed");
      }
    }
    
    // Handle square B
    if (pieceChangedB) {
      if (changeB > 0) {
        readerB.readSignature(signatureB);
        labelB = readerB.identifySticker();
        Serial.println("Square B: Piece placed");
      } else {
        labelB = "Empty";
        Serial.println("Square B: Piece removed");
      }
    }
    
    // Handle square C
    if (pieceChangedC) {
      if (changeC > 0) {
        readerC.readSignature(signatureC);
        labelC = readerC.identifySticker();
        Serial.println("Square C: Piece placed");
      } else {
        labelC = "Empty";
        Serial.println("Square C: Piece removed");
      }
    }
    
    // Handle square D
    if (pieceChangedD) {
      if (changeD > 0) {
        readerD.readSignature(signatureD);
        labelD = readerD.identifySticker();
        Serial.println("Square D: Piece placed");
      } else {
        labelD = "Empty";
        Serial.println("Square D: Piece removed");
      }
    }
    
    // Get FEN characters for each square
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
    
    // Print detailed information only for changed squares that had pieces placed
    // No need to do full info print for removed pieces
    if (changeA > 0) {
      Serial.println("Square A details:");
      printSquareInfo(readerA, signatureA, labelA);
    }
    
    if (changeB > 0) {
      Serial.println("Square B details:");
      printSquareInfo(readerB, signatureB, labelB);
    }
    
    if (changeC > 0) {
      Serial.println("Square C details:");
      printSquareInfo(readerC, signatureC, labelC);
    }
    
    if (changeD > 0) {
      Serial.println("Square D details:");
      printSquareInfo(readerD, signatureD, labelD);
    }
    
    Serial.println("=====================================");
  } else {
    // If no change, just do a quick check every 200ms
    delay(200);
  }
}