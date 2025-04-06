#include <Arduino.h>
#include "StickerReader.h"
#include "LEDUtils.h"

// Example sticker database (calibrated manually later)
StickerSignature stickerDB[] = {
    {"Empty",   {0, 0, 0}},
    {"Red",   {72, 5, 9}},
    {"LightGreen", {2, 41, 10}},
    {"Blue",  {5, 65, 110}},
    {"Gold", {107, 126, 128}},
    {"LightBlue", {31, 99, 165}},
    {"Brown", {21, 7, 9}},
    {"Pink", {100, 19, 91}}, 
    {"Silver", {82, 118, 188}},
    {"Purple", {23, 14, 68}},
    {"Gray", {18, 22, 33}},
    {"Green", {11, 53, 11}},
    {"Black", {3, 4, 7}}
};

// FEN mapping. White is uppercase, black is lowercase PNBRQK
FenMapping pieceMap[] = {
    {"Empty", "-"},
    {"Red",   "p"},
    {"Green", "P"},
    {"Blue",  "Q"},
    {"Gold",  "q"},
    {"LightBlue", "r"},
    {"Brown", "R"},
    {"Pink", "B"},
    {"Silver", "N"},
    {"Purple", "b"},
    {"Gray", "n"},
    {"LightGreen", "K"},
    {"Black", "k"},
};

const int STICKER_DB_SIZE = sizeof(stickerDB) / sizeof(StickerSignature);

StickerReader::StickerReader(int sensorPin, int ledPin, 
                            int redPin, int greenPin, 
                            int bluePin, int irPin,
                            int hallPin, int hallLowThreshold,
                            int hallHighThreshold)
    : _sensorPin(sensorPin), 
    _ledPin(ledPin), 
    _redPin(redPin), 
    _greenPin(greenPin), 
    _bluePin(bluePin), 
    _irPin(irPin), 
    _hallHighThreshold(hallHighThreshold), 
    _hallLowThreshold(hallLowThreshold), 
    _hallPin(hallPin) {}

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
    _irValue = readIRSensor(_sensorPin, _irPin);    // IR

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

// Calculate the distance between two color signatures
// using a weighted Euclidean distance formula
// This is a simple approximation and can be adjusted
// based on the specific characteristics of the colors
// and the sensor used.
// The weights are chosen based on the sensitivity of the sensor
// to different colors. We may need to calibrate these
// weights based on your specific setup and the colors
// we are using.
int StickerReader::distance(int* a, int* b) {
    float weights[3] = {1.0, 1.0, 1.0}; // weights if needed for R, G, B
    float sum = 0;
    for (int i = 0; i < 3; i++) {
        float d = a[i] - b[i];
        sum += weights[i] * d * d;
    }
    return sqrt(sum);
}



const char* StickerReader::getFENFromLabel(const char* label) {
    for (int i = 0; i < sizeof(pieceMap) / sizeof(FenMapping); i++) {
      if (strcmp(label, pieceMap[i].label) == 0) {
        return pieceMap[i].piece;
      }
    }
    return "?";
  }

bool StickerReader::isPiecePresent() {
// int irValue = readIRSensor(_sensorPin, _irPin);
return _irValue > 600; // You can calibrate this
}

const char* StickerReader::identifySticker() {
    int sig[4];
    readSignature(sig);

    const char* closestLabel = "Processing";
    int minDist = 1000000;

    for (int i = 0; i < STICKER_DB_SIZE; i++) {
        int d = distance(sig, stickerDB[i].values);
        if (d < minDist) {
            minDist = d;
            closestLabel = stickerDB[i].label;
        }
    }

    return closestLabel;
}

int StickerReader::getIRValue() const { return _irValue; }
int StickerReader::getHallValue() const { return _hallValue; }
PieceType StickerReader::getPieceType() const { return _pieceType; }
