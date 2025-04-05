#ifndef HALL_SENSOR_H
#define HALL_SENSOR_H

class HallSensor {
public:
    HallSensor(int pin, int threshold);

    void begin();
    int readRaw();
    bool isMagnetDetected();

private:
    int _pin;
    int _threshold;
};

#endif
