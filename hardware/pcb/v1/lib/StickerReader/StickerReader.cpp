#include <Arduino.h>
#include "StickerReader.h"
#include "LEDUtils.h"

// Example sticker database (calibrated manually later)
StickerSignature stickerDB[] = {
    {"Red",   {850, 200, 150, 300}},
    {"Green", {200, 800, 150, 290}},
    {"Blue",  {200, 190, 850, 320}},
    {"White", {900, 900, 900, 900}},
    {"Black", {100, 100, 100, 150}}
};
const int STICKER_DB_SIZE = sizeof(stickerDB) / sizeof(StickerSignature);

StickerReader::StickerReader(int sensorPin, int ledPin, int redPin, int greenPin, int bluePin, int irPin)
    : _sensorPin(sensorPin), _ledPin(ledPin), 
    _redPin(redPin), _greenPin(greenPin), _bluePin(bluePin), _irPin(irPin) {}

void StickerReader::begin() {
    pinMode(_redPin, OUTPUT);
    pinMode(_greenPin, OUTPUT);
    pinMode(_bluePin, OUTPUT);
    pinMode(_irPin, OUTPUT);
    pinMode(_ledPin, OUTPUT);
}

void StickerReader::readSignature(int* out) {
    out[0] = readLEDSensors(_sensorPin, _ledPin, 0, 255, 255);   // R
    out[1] = readLEDSensors(_sensorPin, _ledPin, 255, 0, 255); // G
    out[2] = readLEDSensors(_sensorPin, _ledPin, 255, 255, 0);  // B
    out[3] = readIRSensor(_sensorPin, _irPin);    // IR
}

int StickerReader::distance(int* a, int* b) {
    long sum = 0;
    for (int i = 0; i < 4; i++) {
        long d = (long)a[i] - (long)b[i];
        sum += d * d;
    }
    return (int)sqrt(sum);
}

const char* StickerReader::identifySticker() {
    int sig[4];
    readSignature(sig);

    const char* closestLabel = "Unknown";
    int minDist = 100000;

    for (int i = 0; i < STICKER_DB_SIZE; i++) {
        int d = distance(sig, stickerDB[i].values);
        if (d < minDist) {
            minDist = d;
            closestLabel = stickerDB[i].label;
        }
    }

    return closestLabel;
}
