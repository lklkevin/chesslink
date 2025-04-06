#include <Arduino.h>

#define HALL_SENSOR_PIN A4  // Analog input pin, A4, A5, A6, or A7
#define THRESHOLD 500       // Adjust based on your sensor’s baseline

void setup() {
    Serial.begin(9600);
}

void loop() {
    int sensorValue = analogRead(HALL_SENSOR_PIN);
    Serial.print("Sensor Value: ");
    Serial.println(sensorValue);

    // if (sensorValue < THRESHOLD) { // Adjust condition based on sensor behavior
    //     Serial.println("Magnet Detected!");
    // } else {
    // }

    delay(500);
}