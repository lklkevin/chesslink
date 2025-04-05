#ifndef SQUARE_UNIT_H
#define SQUARE_UNIT_H

#include <Arduino.h>

enum PieceType {
    PIECE_NONE,
    PIECE_WHITE,
    PIECE_BLACK
};

class SquareUnit {
public:
    SquareUnit(int photoPin, int hallPin, int ledEnablePin, int hallLowThreshold = 300, int hallHighThreshold = 500);

    void begin();
    void readSensors();
    void printStatus();
    void setColor(int r, int g, int b);
    void turnOff();

    int getPhotoValue() const;
    int getHallValue() const;
    PieceType getPieceType() const;
    char detectPiece();
    bool isMagnetDetected() const { return _pieceType != PIECE_NONE; }

private:
    int _photoPin;
    int _hallPin;
    int _ledEnablePin;
    int _hallLowThreshold;
    int _hallHighThreshold;
    int _photoValue;
    int _hallValue;
    PieceType _pieceType;
};

#endif
