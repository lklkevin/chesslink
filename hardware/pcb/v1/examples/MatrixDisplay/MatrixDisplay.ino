#include <Arduino.h>
#include "SquareUnit.h"
#include "SensorUtils.h"

// Define pins for each unit (adjust these values based on your actual pin configuration)
// Format: {photoPin, hallPin, ledEnablePin, hallLowThreshold, hallHighThreshold}
const int UNIT9_PINS[] = {A2, A5, 9, 400, 600};  // Top Right
const int UNIT10_PINS[] = {A1, A6, 10, 400, 600}; // Top Left
const int UNIT11_PINS[] = {A0, A7, 11, 400, 600}; // Bottom Left
const int UNIT12_PINS[] = {A3, A4, 12, 400, 600}; // Bottom Right

// Create SquareUnit objects
SquareUnit unit9(UNIT9_PINS[0], UNIT9_PINS[1], UNIT9_PINS[2], UNIT9_PINS[3], UNIT9_PINS[4]);
SquareUnit unit10(UNIT10_PINS[0], UNIT10_PINS[1], UNIT10_PINS[2], UNIT10_PINS[3], UNIT10_PINS[4]);
SquareUnit unit11(UNIT11_PINS[0], UNIT11_PINS[1], UNIT11_PINS[2], UNIT11_PINS[3], UNIT11_PINS[4]);
SquareUnit unit12(UNIT12_PINS[0], UNIT12_PINS[1], UNIT12_PINS[2], UNIT12_PINS[3], UNIT12_PINS[4]);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  // Initialize all units
  unit9.begin();
  unit10.begin();
  unit11.begin();
  unit12.begin();
  
  Serial.println("2x2 Chess Board Matrix Display");
  Serial.println("-----------------------------");
}

// Helper function to get a character representation of the piece
char getPieceChar(PieceType type) {
  switch (type) {
    case PIECE_WHITE: return 'W';
    case PIECE_BLACK: return 'B';
    default: return '.';
  }
}

// Function prototype for printPieceStatus
void printPieceStatus(const SquareUnit& unit) {
  Serial.print("Hall=");
  Serial.print(unit.getHallValue());
  Serial.print(", Photo=");
  Serial.print(unit.getPhotoValue());
  Serial.print(", Piece=");
  
  switch (unit.getPieceType()) {
    case PIECE_WHITE: Serial.println("WHITE"); break;
    case PIECE_BLACK: Serial.println("BLACK"); break;
    default: Serial.println("NONE"); break;
  }
}

void loop() {
  // Read sensors for all units
  unit9.readSensors();
  unit10.readSensors();
  unit11.readSensors();
  unit12.readSensors();
  
  // Clear the console (not ideal but helps visualization)
  Serial.write(27);     // ESC command
  Serial.print("[2J");  // clear screen command
  Serial.write(27);
  Serial.print("[H");   // cursor to home command
  
  Serial.println("2x2 Chess Board Matrix:");
  Serial.println("+---+---+");
  
  // Print top row (units 10 and 9)
  Serial.print("| ");
  Serial.print(getPieceChar(unit10.getPieceType()));
  Serial.print(" | ");
  Serial.print(getPieceChar(unit9.getPieceType()));
  Serial.println(" |");
  
  Serial.println("+---+---+");
  
  // Print bottom row (units 11 and 12)
  Serial.print("| ");
  Serial.print(getPieceChar(unit11.getPieceType()));
  Serial.print(" | ");
  Serial.print(getPieceChar(unit12.getPieceType()));
  Serial.println(" |");
  
  Serial.println("+---+---+");
  
  // Detailed sensor readings (optional)
  Serial.println("\nDetailed Readings:");
  Serial.print("Unit 9 (Top Right): ");
  printPieceStatus(unit9);
  Serial.print("Unit 10 (Top Left): ");
  printPieceStatus(unit10);
  Serial.print("Unit 11 (Bottom Left): ");
  printPieceStatus(unit11);
  Serial.print("Unit 12 (Bottom Right): ");
  printPieceStatus(unit12);
  
  delay(1000);  // Update every second
}
