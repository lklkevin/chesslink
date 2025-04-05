#include <Arduino.h>

const int photoPinA = A0;  // Analog input pin where the photodiode is connected
const int photoPinB = A1;  // Analog input pin where the photodiode is connected
const int photoPinC = A2;  // Analog input pin where the photodiode is connected
const int photoPinD = A3;  // Analog input pin where the photodiode is connected

int sensorValueA = 200;      // Variable to store the value read from the photodiode
int sensorValueB = 200;      // Variable to store the value read from the photodiode
int sensorValueC = 200;      // Variable to store the value read from the photodiode
int sensorValueD = 200;      // Variable to store the value read from the photodiode

const int RGB_A_ON = 12;  // Enable pin. We have 9 thorugh 12

const int IR_LED = 2; // IR LED is at 2

const int redPin = 6;    // Red cathode
const int greenPin = 3;  // Green cathode
const int bluePin = 5;   // Blue cathode

void setup() {
  Serial.begin(9600);  // Start the serial communication

  pinMode(IR_LED, OUTPUT);

  pinMode(RGB_A_ON, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  digitalWrite(RGB_A_ON, HIGH);  // Enable the LED
}

void loop() {
  digitalWrite(RGB_A_ON, HIGH); // Enable it again

  analogWrite(redPin, 255);   // Full Red ON
  analogWrite(greenPin, 255); // Full Green ON
  analogWrite(bluePin, 0);  // Full Blue ON

  sensorValueA = analogRead(photoPinA);  // Read the analog input
  sensorValueB = analogRead(photoPinB);  // Read the analog input
  sensorValueC = analogRead(photoPinC);  // Read the analog input
  sensorValueD = analogRead(photoPinD);  // Read the analog input

  Serial.print("A0: ");
  Serial.print(sensorValueA);
  Serial.print(", A1: ");
  Serial.print(sensorValueB);
  Serial.print(", A2: ");
  Serial.print(sensorValueC);
  Serial.print(", A3: ");
  Serial.println(sensorValueD);  // Use println() to end the line
  
  delay(100);  // Wait for 100ms before the next reading

  // delay(1000);

  // analogWrite(redPin, 0);   // Turn Red ON (because it's common anode)
  // analogWrite(greenPin, 255); // Turn Green OFF
  // analogWrite(bluePin, 255);  // Turn Blue OFF
  // delay(1000);

  // digitalWrite(RGB_A_ON, LOW);  // Turn OFF the LED completely
  // delay(1000);

  // digitalWrite(RGB_A_ON, HIGH); // Enable it again
  // analogWrite(redPin, 255);   // Turn Red OFF
  // analogWrite(greenPin, 0);   // Turn Green ON
  // analogWrite(bluePin, 255);  // Turn Blue OFF
  // delay(1000);

  // digitalWrite(RGB_A_ON, LOW);  // Turn OFF the LED completely
  // delay(1000);

  // digitalWrite(RGB_A_ON, HIGH); // Enable it again
  // analogWrite(redPin, 255);   // Turn Red OFF
  // analogWrite(greenPin, 255);   // Turn Green ON
  // analogWrite(bluePin, 0);  // Turn Blue OFF
  // delay(1000);

  // digitalWrite(RGB_A_ON, LOW);  // Turn OFF the LED completely
  // delay(1000);
}
