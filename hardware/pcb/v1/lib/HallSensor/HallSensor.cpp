#include <Arduino.h>
#include "HallSensor.h"

HallSensor::HallSensor(int pin, int threshold) {
    _pin = pin;
    _threshold = threshold;
}

void HallSensor::begin() {
    pinMode(_pin, INPUT);
}

int HallSensor::readRaw() {
    return analogRead(_pin);
}

bool HallSensor::isMagnetDetected() {
    return readRaw() > _threshold;
}
