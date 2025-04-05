#include <Arduino.h>
#include <SquareUnit.h>

#define IR_EMITTER_PIN 2

// Sensor Pins 
SquareUnit unitA(A2, A5, 9);
SquareUnit unitB(A1, A6, 10);
SquareUnit unitC(A0, A7, 11);
SquareUnit unitD(A3, A4, 12);

SquareUnit* units[] = { &unitA, &unitB, &unitC, &unitD };

void setup() {
  Serial.begin(9600);
  pinMode(IR_EMITTER_PIN, OUTPUT);
  for (auto unit : units) unit->begin();
}

void loop() {
  for (auto unit : units) {
    unit->readSensors();
    unit->printStatus();

    // Optional: visualize result
    if (unit->isMagnetDetected())
      unit->setColor(255, 0, 255);  // Green for magnet
    else
      unit->setColor(0, 255, 255);  // Red otherwise, cathodes
  }

  delay(1000);
}
