#include <Arduino.h>
#include "SquareUnit.h"
#include "LEDUtils.h"

const int RED_PIN = 6;
const int GREEN_PIN = 3;
const int BLUE_PIN = 5;

SquareUnit::SquareUnit(int photoPin, int hallPin, int ledEnablePin, int hallLowThreshold, int hallHighThreshold)
    : _photoPin(photoPin), _hallPin(hallPin), _ledEnablePin(ledEnablePin), 
      _hallLowThreshold(hallLowThreshold), _hallHighThreshold(hallHighThreshold), _pieceType(PIECE_NONE) {}

void SquareUnit::begin() {
    pinMode(_ledEnablePin, OUTPUT);
    pinMode(_hallPin, INPUT);
    digitalWrite(_ledEnablePin, HIGH);
}

void SquareUnit::readSensors() {
    _photoValue = readIRSensor(_photoPin, 2);  // IR emitter is on pin 2 globally
    _hallValue = analogRead(_hallPin);
    
    // Determine piece type based on hall sensor value
    if (_hallValue > _hallHighThreshold) {
        _pieceType = PIECE_WHITE;
    } else if (_hallValue < _hallLowThreshold) {
        _pieceType = PIECE_BLACK;
    } else {
        _pieceType = PIECE_NONE;
    }
}

char SquareUnit::detectPiece() {
    if (_pieceType == PIECE_NONE) return ' ';
  
    // Simplified logic using hall + photo sensor values
    // You can adjust these ranges based on real readings
  
    bool isWhite = (_photoValue > 600); // bright = white piece
    int pieceID = map(_hallValue, 300, 900, 1, 6); // crude analog profile
    pieceID = constrain(pieceID, 1, 6);
  
    char piece = ' ';
  
    switch (pieceID) {
      case 1: piece = 'P'; break;
      case 2: piece = 'N'; break;
      case 3: piece = 'B'; break;
      case 4: piece = 'R'; break;
      case 5: piece = 'Q'; break;
      case 6: piece = 'K'; break;
    }
  
    if (!isWhite) piece = tolower(piece);
    return piece;
  }
  

void SquareUnit::printStatus() {
    Serial.print("unit: ");
    Serial.print(_ledEnablePin);  // identify by enable pin
    Serial.print(", photo:");
    Serial.print(_photoValue);
    Serial.print(", hall:");
    Serial.print(_hallValue);
    Serial.print(", piece:");
    
    switch (_pieceType) {
        case PIECE_WHITE: Serial.println("WHITE"); break;
        case PIECE_BLACK: Serial.println("BLACK"); break;
        default: Serial.println("NONE"); break;
    }
}

void SquareUnit::setColor(int r, int g, int b) {
    digitalWrite(_ledEnablePin, HIGH);  // Turn on RGB
    analogWrite(RED_PIN, r);
    analogWrite(GREEN_PIN, g);
    analogWrite(BLUE_PIN, b);
}

void SquareUnit::turnOff() {
    digitalWrite(_ledEnablePin, LOW);  // Disable RGB LED
}

int SquareUnit::getPhotoValue() const { return _photoValue; }
int SquareUnit::getHallValue() const { return _hallValue; }
PieceType SquareUnit::getPieceType() const { return _pieceType; }
