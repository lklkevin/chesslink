#include <Arduino.h>
#include "SensorUtils.h"

int readCleanSensor(int pin, int emitterPin) {
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

  return max(0, signal);
}
