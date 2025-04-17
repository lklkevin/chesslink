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
    bool isPiecePresent();
    int getIRValue() const;
    
    // New methods for ambient light detection
    int readAmbient();
    // bool hasAmbientChanged(int threshold = 1); // Kept for backward compatibility
    int checkAmbientChange(int threshold = 1); // New method: returns 1 (placed), -1 (removed), 0 (no change)

private:
    int _sensorPin;
    int _redPin, _greenPin, _bluePin, _irPin;
    int _ledPin;
    int _photoPin;
    int _hallPin;
    int _hallLowThreshold;
    int _hallHighThreshold;
    int _hallValue;
    int _irValue;
    PieceType _pieceType;
    int _lastAmbient; // To track previous ambient reading
    unsigned long _lastAmbientTime; // To track when we last measured ambient

    int distance(int* a, int* b);
};

#endif
