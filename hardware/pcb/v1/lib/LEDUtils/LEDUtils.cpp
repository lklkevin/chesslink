#include <Arduino.h>
#include "LEDUtils.h"

int readLEDSensors(int photoDiodePin, int ledPin, int r, int g, int b) {
  digitalWrite(ledPin, LOW);

  // off
  analogWrite(RED_PIN, 255);
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 255);

  delayMicroseconds(300);
  int off1 = analogRead(photoDiodePin);

  digitalWrite(ledPin, HIGH);

  // set to on
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);

  delayMicroseconds(500);
  int on = analogRead(photoDiodePin);

  digitalWrite(ledPin, LOW);

  // off
  analogWrite(RED_PIN, 255);
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 255);
  
  delayMicroseconds(300);
  int off2 = analogRead(photoDiodePin);

  float ambient = (off1 + off2) / 2.0;
  int signal = on - ambient;

  // Print debug information
  // Serial.print("off1: "); Serial.print(off1);
  // Serial.print(" | on: "); Serial.print(on);
  // Serial.print(" | off2: "); Serial.print(off2);
  // Serial.print(" | ambient: "); Serial.print(ambient);
  // Serial.print(" | signal: "); Serial.println(signal);

  return max(0, signal);
}



int readIRSensor(int pin, int emitterPin) {
  digitalWrite(emitterPin, LOW);
  delayMicroseconds(300);
  int off1 = analogRead(pin);

  digitalWrite(emitterPin, HIGH);
  delayMicroseconds(300);
  int on = analogRead(pin);

  digitalWrite(emitterPin, LOW);
  delayMicroseconds(300);
  int off2 = analogRead(pin);

  float ambient = (off1 + off2) / 2.0;
  int signal = on - ambient;

  // Print debug information
  // Serial.print("off1: "); Serial.print(off1);
  // Serial.print(" | on: "); Serial.print(on);
  // Serial.print(" | off2: "); Serial.print(off2);
  // Serial.print(" | ambient: "); Serial.print(ambient);
  // Serial.print(" | signal: "); Serial.println(signal);

  return max(0, signal);
}

int readAmbientLight(int photoDiodePin) {
  // Make sure all LEDs are off
  analogWrite(RED_PIN, 255);
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 255);
  
  long total = 0;
  int numReadings = 10;  // You can adjust this based on how stable you need the reading

  // Take 'numReadings' readings and accumulate the results
  for (int i = 0; i < numReadings; i++) {
    total += analogRead(photoDiodePin);
    delayMicroseconds(1000);  // Small delay between readings to allow sensor settling
  }

  // Calculate the average of the readings
  int ambient = total / numReadings;

  return ambient;
}
