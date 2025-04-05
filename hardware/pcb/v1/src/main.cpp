#include <Arduino.h>
#include <SensorUtils.h>

const int photoPinA = A2; 
const int photoPinB = A1; 
const int photoPinC = A0; 
const int photoPinD = A3; 

int sensorValueA = 200;    
int sensorValueB = 200;   
int sensorValueC = 200;   
int sensorValueD = 200;  

const int RGB_A_ON = 12;
const int RGB_B_ON = 12; 

const int redPin = 6;    // Red cathode
const int greenPin = 3;  // Green cathode
const int bluePin = 5;   // Blue cathode

const int IR_EMITTER_PIN = 2;

void setup() {
  Serial.begin(9600);
  pinMode(IR_EMITTER_PIN, OUTPUT);
}

void loop() {
  int signal = readCleanSensor(photoPinD, IR_EMITTER_PIN);
  Serial.println(signal);
  delay(100);
}
