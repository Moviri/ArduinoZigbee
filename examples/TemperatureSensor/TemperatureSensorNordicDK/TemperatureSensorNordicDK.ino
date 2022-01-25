/*
  Zigbee HA Temperature Sensor example

  This example creates a zigbee device following the HA(Home Automation) profile.
  It simulates a temperature sensor read.

  The circuit:
  - nRF52840 DK

  This example code is in the public domain.
*/

#include <ArduinoZigbee.h>

float tempSensorCB_1() {
  // Put sensor read here
  return 28.0; 
}

#define EP_1 15

TemperatureSensor(temp_1, EP_1, tempSensorCB_1, 2000);

void setup() {
  ZIGBEE.begin(1 << 25);
}
void loop() {
  ZIGBEE.poll();
}
