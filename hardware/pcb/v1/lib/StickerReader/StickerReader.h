#ifndef STICKER_READER_H
#define STICKER_READER_H

struct StickerSignature {
    const char* label;
    int values[4]; // [Red, Green, Blue, IR]
};

class StickerReader {
public:
    StickerReader(int sensorPin, int ledPin,
                int redPin, int greenPin, int bluePin, int irPin);

    void begin();
    void readSignature(int* out);
    const char* identifySticker();

private:
    int _sensorPin;
    int _redPin, _greenPin, _bluePin, _irPin;
    int _ledPin;

    int distance(int* a, int* b);
};

#endif
