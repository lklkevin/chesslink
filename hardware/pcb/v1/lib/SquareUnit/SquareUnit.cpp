#include <Arduino.h>
#include "SquareUnit.h"
#include "SensorUtils.h"

const int RED_PIN = 6;
const int GREEN_PIN = 3;
const int BLUE_PIN = 5;

SquareUnit::SquareUnit(int photoPin, int hallPin, int ledEnablePin, int hallThreshold)
    : _photoPin(photoPin), _hallPin(hallPin), _ledEnablePin(ledEnablePin), _hallThreshold(hallThreshold) {}

void SquareUnit::begin() {
    pinMode(_ledEnablePin, OUTPUT);
    pinMode(_hallPin, INPUT);
    digitalWrite(_ledEnablePin, HIGH);
}

void SquareUnit::readSensors() {
    _photoValue = readCleanSensor(_photoPin, 2);  // IR emitter is on pin 2 globally
    _hallValue = analogRead(_hallPin);
    _magnetDetected = _hallValue < _hallThreshold;
}

void SquareUnit::printStatus() {
    Serial.print("unit:");
    Serial.print(_ledEnablePin);  // identify by enable pin
    Serial.print(", photo:");
    Serial.print(_photoValue);
    Serial.print(", hall:");
    Serial.print(_hallValue);
    Serial.print(", magnet:");
    Serial.println(_magnetDetected);
}

void SquareUnit::setColor(int r, int g, int b) {
    digitalWrite(_ledEnablePin, HIGH);  // Turn on RGB
    analogWrite(RED_PIN, r);
    analogWrite(GREEN_PIN, g);
    analogWrite(BLUE_PIN, b);
}

int SquareUnit::getPhotoValue() const { return _photoValue; }
int SquareUnit::getHallValue() const { return _hallValue; }
bool SquareUnit::isMagnetDetected() const { return _magnetDetected; }
