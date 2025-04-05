#ifndef SENSOR_UTILS_H
#define SENSOR_UTILS_H

const int RED_PIN = 6;
const int GREEN_PIN = 3;
const int BLUE_PIN = 5;

int readLEDSensors(int pin, int emitterPin, int r, int g, int b);
int readIRSensor(int pin, int emitterPin);

#endif
