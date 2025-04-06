#include <Arduino.h>
#include "LEDUtils.h"

int readLEDSensors(int photoDiodePin, int ledPin, int r, int g, int b) {
  // First turn off the LED and set RGB to off (255 for common anode)
  digitalWrite(ledPin, LOW);
  analogWrite(RED_PIN, 255);
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 255);
  delayMicroseconds(300);
  int off1 = analogRead(photoDiodePin);

  // Turn on the LED with the specified RGB color
  digitalWrite(ledPin, HIGH);
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
  delayMicroseconds(500);
  int on = analogRead(photoDiodePin);

  // Turn off the LED again
  digitalWrite(ledPin, LOW);
  analogWrite(RED_PIN, 255);
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 255);
  delayMicroseconds(300);
  int off2 = analogRead(photoDiodePin);

  // Calculate the ambient light and signal
  float ambient = (off1 + off2) / 2.0;
  int signal = on - ambient;

  // Return the signal (ensure it's not negative)
  return max(0, signal);
}

int readIRSensor(int pin, int emitterPin) {
  // Off reading 1
  digitalWrite(emitterPin, LOW);
  delayMicroseconds(300);
  int off1 = analogRead(pin);

  // On reading
  digitalWrite(emitterPin, HIGH);
  delayMicroseconds(300);
  int on = analogRead(pin);

  // Off reading 2
  digitalWrite(emitterPin, LOW);
  delayMicroseconds(300);
  int off2 = analogRead(pin);

  // Calculate ambient light and signal
  float ambient = (off1 + off2) / 2.0;
  int signal = on - ambient;

  return max(0, signal);
}

// New function to read multiple sensors simultaneously
void readAllSensors(int numSensors, int* photoDiodePins, int* ledPins, int* results, int r, int g, int b) {
  // Turn off all LEDs
  for (int i = 0; i < numSensors; i++) {
    digitalWrite(ledPins[i], LOW);
  }
  
  // Set RGB to off (255 for common anode)
  analogWrite(RED_PIN, 255);
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 255);
  delayMicroseconds(300);
  
  // Read ambient light for all sensors
  int off1[numSensors];
  for (int i = 0; i < numSensors; i++) {
    off1[i] = analogRead(photoDiodePins[i]);
  }
  
  // Turn on all LEDs
  for (int i = 0; i < numSensors; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
  
  // Set RGB color
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
  delayMicroseconds(500);
  
  // Read signals with LEDs on
  int on[numSensors];
  for (int i = 0; i < numSensors; i++) {
    on[i] = analogRead(photoDiodePins[i]);
  }
  
  // Turn off all LEDs
  for (int i = 0; i < numSensors; i++) {
    digitalWrite(ledPins[i], LOW);
  }
  
  // Set RGB to off again
  analogWrite(RED_PIN, 255);
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 255);
  delayMicroseconds(300);
  
  // Read ambient light again
  int off2[numSensors];
  for (int i = 0; i < numSensors; i++) {
    off2[i] = analogRead(photoDiodePins[i]);
  }
  
  // Calculate final results
  for (int i = 0; i < numSensors; i++) {
    float ambient = (off1[i] + off2[i]) / 2.0;
    results[i] = max(0, on[i] - ambient);
  }
}

// New function to read IR for multiple sensors simultaneously
void readAllIRSensors(int numSensors, int* photoDiodePins, int irPin, int* results) {
  // Turn off IR
  digitalWrite(irPin, LOW);
  delayMicroseconds(300);
  
  // Read ambient for all sensors
  int off1[numSensors];
  for (int i = 0; i < numSensors; i++) {
    off1[i] = analogRead(photoDiodePins[i]);
  }
  
  // Turn on IR
  digitalWrite(irPin, HIGH);
  delayMicroseconds(300);
  
  // Read with IR on
  int on[numSensors];
  for (int i = 0; i < numSensors; i++) {
    on[i] = analogRead(photoDiodePins[i]);
  }
  
  // Turn off IR
  digitalWrite(irPin, LOW);
  delayMicroseconds(300);
  
  // Read ambient again
  int off2[numSensors];
  for (int i = 0; i < numSensors; i++) {
    off2[i] = analogRead(photoDiodePins[i]);
  }
  
  // Calculate final results
  for (int i = 0; i < numSensors; i++) {
    float ambient = (off1[i] + off2[i]) / 2.0;
    results[i] = max(0, on[i] - ambient);
  }
}
