#include <Arduino.h>
#include "SquareUnit.h"
#include "SensorUtils.h"

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
    _photoValue = readCleanSensor(_photoPin, 2);  // IR emitter is on pin 2 globally
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

void SquareUnit::printStatus() {
    Serial.print("unit:");
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

int SquareUnit::getPhotoValue() const { return _photoValue; }
int SquareUnit::getHallValue() const { return _hallValue; }
PieceType SquareUnit::getPieceType() const { return _pieceType; }
