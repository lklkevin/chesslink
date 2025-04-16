#include <Arduino.h>
#include "StickerReader.h"
#include "LEDUtils.h"

// Example sticker database (calibrated manually later)
StickerSignature stickerDB_1kohm_1mm_1kohm[] = {
    {"Empty",   {0, 0, 0}},
    {"Red",   {130, 11, 18}},
    {"LightGreen", {2, 41, 10}},
    {"Blue",  {5, 65, 110}},
    {"Gold", {107, 126, 128}},
    {"LightBlue", {31, 99, 165}},
    {"Brown", {21, 7, 9}},
    {"Pink", {100, 19, 91}}, 
    {"Yellow", {141, 128, 27}},
    {"Purple", {23, 14, 68}},
    {"Gray", {40, 46, 76}},
    {"Green", {11, 53, 11}},
    {"Orange", {125, 39, 21}}
};

// Example sticker database (calibrated manually later)
StickerSignature stickerDB_4k_ohm_1mm[] = {
    {"Empty",  {0, 0, 0}},
    {"Red",   {605, 54, 93}},
    {"LightGreen", {123, 513, 146}},
    {"Blue",  {52, 336, 676}},
    {"Gold", {513, 583, 620}},
    {"LightBlue", {227, 653, 966}},
    {"Brown", {150, 69, 97}},
    {"Pink", {607, 119, 576}}, 
    {"Yellow", {703, 629, 138}},
    {"Purple", {177, 106, 502}},
    {"Gray", {193, 218, 364}},
    {"Green", {36, 338, 114}},
    {"Orange", {612, 195, 111}}
};


// Example sticker database (calibrated manually later)
StickerSignature stickerDB_5mm_1mm[] = {
    {"Empty",  {5, 5, 15}},
    {"Red",   {33, 9, 13}},
    {"LightGreen", {11, 30, 16}},
    {"Blue",  {6, 25, 42}},
    {"Gold", {25, 34, 32}},
    {"LightBlue", {14, 34, 53}},
    {"LightPink", {27, 19, 39}},
    {"Pink", {34, 12, 37}}, 
    {"Yellow", {39, 39, 14}},
    {"Purple", {10, 10, 25}},
    {"White", {30, 42, 71}},
    {"Green", {8, 27, 17}},
    {"Orange", {29, 19, 15}}
};



// FEN mapping. White is uppercase, black is lowercase PNBRQK
FenMapping pieceMap[] = {
    {"Empty", "-"},
    {"Red",   "p"},
    {"Green", "P"},
    {"Blue",  "Q"},
    {"Gold",  "q"},
    {"LightBlue", "r"},
    {"LightPink", "R"},
    {"Pink", "B"},
    {"Yellow", "N"},
    {"Purple", "b"},
    {"White", "n"},
    {"LightGreen", "K"},
    {"Orange", "k"},
};

const int STICKER_DB_SIZE = sizeof(stickerDB_5mm_1mm) / sizeof(StickerSignature);

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
    _hallPin(hallPin),
    _lastAmbient(0),
    _lastAmbientTime(0) {}

void StickerReader::begin() {
    pinMode(_redPin, OUTPUT);
    pinMode(_greenPin, OUTPUT);
    pinMode(_bluePin, OUTPUT);
    pinMode(_irPin, OUTPUT);
    pinMode(_ledPin, OUTPUT);
    
    // Initialize ambient light reading
    _lastAmbient = readAmbient();
    _lastAmbientTime = millis();
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

// Read current ambient light level
int StickerReader::readAmbient() {
    int ambient = readAmbientLight(_sensorPin);
    _lastAmbient = ambient;
    _lastAmbientTime = millis();
    return ambient;
}

// Check if ambient light has changed beyond threshold
// Return values: 0 = no significant change, 1 = piece placed, -1 = piece removed
int StickerReader::checkAmbientChange(int threshold) {
    int currentAmbient = readAmbientLight(_sensorPin);
    // int diff = currentAmbient - _lastAmbient;
    
    // Update the last ambient light reading regardless of change detection
    // _lastAmbientTime = millis();

    Serial.print("current ambient: ");
    Serial.println(currentAmbient);

    // Return 1 if light increased (piece placed), -1 if decreased (piece removed), 0 if no significant change
    if (currentAmbient > 6 && _lastAmbient <= 6) {
        _lastAmbient = currentAmbient;
        return -1;  // Piece removed (light increased)
    } else if (currentAmbient <= 6) {
        _lastAmbient = currentAmbient;

        return 1; // Piece added (light decreased)
    } else {
        return 0;  // No significant change
    }
}

// void normalize(int* color, float* normColor) {
//     float length = sqrt(color[0]*color[0] + color[1]*color[1] + color[2]*color[2]);
//     if (length == 0) length = 1; // avoid division by zero
//     for (int i = 0; i < 3; ++i) {
//         normColor[i] = (float)color[i] / length;
//     }
// }

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
    float weights[3] = {1.0, 1.0, 0.8}; // weights if needed for R, G, B
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
        int d = distance(sig, stickerDB_5mm_1mm[i].values);
        if (d < minDist) {
            minDist = d;
            closestLabel = stickerDB_5mm_1mm[i].label;
        }
    }

    return closestLabel;
}

int StickerReader::getIRValue() const { return _irValue; }
int StickerReader::getHallValue() const { return _hallValue; }
PieceType StickerReader::getPieceType() const { return _pieceType; }
