#ifndef SQUARE_UNIT_H
#define SQUARE_UNIT_H

class SquareUnit {
public:
    SquareUnit(int photoPin, int hallPin, int ledEnablePin, int hallThreshold = 500);

    void begin();
    void readSensors();
    void printStatus();   // Print debug info
    void setColor(int r, int g, int b);

    int getPhotoValue() const;
    int getHallValue() const;
    bool isMagnetDetected() const;

private:
    int _photoPin;
    int _hallPin;
    int _ledEnablePin;
    int _hallThreshold;

    int _photoValue;
    int _hallValue;
    bool _magnetDetected;
};

#endif
