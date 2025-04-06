#ifndef STICKER_READER_H
#define STICKER_READER_H

struct StickerSignature {
    const char* label;
    int values[4]; // [Red, Green, Blue, IR]
};

struct FenMapping {
    const char* label;
    const char* piece; // [Red, Green, Blue, IR]
};

enum PieceType {
    PIECE_NONE,
    PIECE_WHITE,
    PIECE_BLACK
};

class StickerReader {
public:
    StickerReader(int sensorPin, int ledPin,
                int redPin, int greenPin, int bluePin, int irPin, int hallPin,
                int hallLowThreshold = 300, int hallHighThreshold = 500);

    void begin();
    void readSignature(int* out);
    const char* identifySticker();
    int getHallValue() const;
    PieceType getPieceType() const;
    const char* getFENFromLabel(const char* label);

private:
    int _sensorPin;
    int _redPin, _greenPin, _bluePin, _irPin;
    int _ledPin;
    int _photoPin;
    int _hallPin;
    int _hallLowThreshold;
    int _hallHighThreshold;
    int _hallValue;
    PieceType _pieceType;

    int distance(int* a, int* b);
};

#endif
