#include <Arduino.h>
#include <unity.h>
#include <SensorUtils.h>

void test_sensor_logic() {
  int result = readCleanSensor(A0, 2); // Real sensor call (if testing on device)
  TEST_ASSERT(result >= 0);
}

void setup() {
  delay(2000); // wait for serial monitor to connect
  UNITY_BEGIN();
  RUN_TEST(test_sensor_logic);
  UNITY_END();
}

void loop() {}
