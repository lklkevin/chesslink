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

// Array of readers for batch operations
StickerReader* readers[4];

// Arrays to store signatures for each reader
int* signatures[4];


// Helper function to print detailed information for each square
void printDetailedInfo() {
  const char* squareNames[4] = {"A", "B", "C", "D"};
  
  for (int i = 0; i < 4; i++) {
    Serial.print("Square ");
    Serial.print(squareNames[i]);
    Serial.println(":");
    
    Serial.print("  R: "); Serial.print(signatures[i][0]);
    Serial.print(", G: "); Serial.print(signatures[i][1]);
    Serial.print(", B: "); Serial.print(signatures[i][2]);
    Serial.print(" => Detected: ");
    
    const char* label = readers[i]->identifySticker();
    Serial.print(label);
    
    Serial.print(" | Hall Value: ");
    Serial.print(readers[i]->getHallValue());
    Serial.print(" | Piece Type: ");
    switch (readers[i]->getPieceType()) {
      case PIECE_WHITE: Serial.println("WHITE"); break;
      case PIECE_BLACK: Serial.println("BLACK"); break;
      default: Serial.println("NONE"); break;
    }
    
    Serial.print("  IR Value: ");
    Serial.print(readers[i]->getIRValue());
    Serial.print(" | Piece Present: ");
    Serial.println(readers[i]->isPiecePresent() ? "YES" : "NO");
  }
}


void setup() {
  Serial.begin(9600);
  
  // // Initialize all readers
  // readerA.begin();
  // readerB.begin();
  // readerC.begin();
  // readerD.begin();
  
  // Initialize reader array for batch operations
  readers[0] = &readerA;
  readers[1] = &readerB;
  readers[2] = &readerC;
  readers[3] = &readerD;
  
  // Initialize signature arrays
  for (int i = 0; i < 4; i++) {
    signatures[i] = new int[4];
  }
  
  Serial.println("All Sticker Readers Initialized");
}

void loop() {
  // Method 1: Read all sensors at once using the static batch methods
  StickerReader::readAllSignatures(4, *readers, signatures);
  
  // Get combined FEN string for all squares
  String combinedFEN = StickerReader::getCombinedFEN(4, *readers);
  
  // Print combined FEN
  Serial.print("Combined FEN: ");
  Serial.println(combinedFEN);
  
  // Print detailed information for debugging
  printDetailedInfo();
  
  Serial.println("=====================================");
  delay(1000);
}
