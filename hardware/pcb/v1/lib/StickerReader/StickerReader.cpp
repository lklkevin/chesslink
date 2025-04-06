#include <Arduino.h>
#include "StickerReader.h"
#include "LEDUtils.h"

// Example sticker database (calibrated manually later)
StickerSignature stickerDB[] = {
    {"Empty",   {0, 0, 0, 478}},
    {"Red",   {72, 5, 9, 716}},
    {"Green", {2, 41, 10, 673}},
    {"Blue",  {5, 65, 110, 659}},
    {"Gold", {107, 126, 128, 758}},
    {"LightBlue", {31, 99, 165, 720}},
    {"White", {103, 141, 236, 783}},
    {"Pink", {100, 19, 91, 796}}, 
    {"Silver", {82, 118, 188, 692}},
    {"Purple", {23, 14, 68, 742}},
    {"Gray", {18, 22, 33, 547}},
    {"LightGreen", {11, 53, 11, 655}},
    {"Black", {3, 4, 7, 494}}
};

// FEN mapping. White is uppercase, black is lowercase PNBRQK
FenMapping pieceMap[] = {
    {"Empty", "-"},
    {"Red",   "p"},
    {"Green", "P"},
    {"Blue",  "Q"},
    {"Gold",  "q"},
    {"LightBlue", "r"},
    {"White", "R"},
    {"Pink", "B"},
    {"Silver", "N"},
    {"Purple", "b"},
    {"Gray", "n"},
    {"LightGreen", "K"},
    {"Black", "k"},
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

const char* StickerReader::getFENFromLabel(const char* label) {
    for (int i = 0; i < sizeof(pieceMap) / sizeof(FenMapping); i++) {
      if (strcmp(label, pieceMap[i].label) == 0) {
        return pieceMap[i].piece;
      }
    }
    return "?";
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
