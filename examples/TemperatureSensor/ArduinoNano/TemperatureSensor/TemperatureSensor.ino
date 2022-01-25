/*
  Zigbee HA Temperature Sensor example

  This example creates a zigbee device following the HA(Home Automation) profile.
  It uses the onboard temperature sensor of an Arduino Nano 33 BLE Sense to read 
  room temperature

  The circuit:
  - Arduino Nano 33 BLE Sense

  This example code is in the public domain.
*/
#include <ArduinoZigbee.h>

float tempSensorCB_1() {
  // Put sensor read here
  return float(random(20.0,28.0)); 
}

#define EP_1 15

TemperatureSensor(temp_1, EP_1, tempSensorCB_1, 2000);

void setup() {
  ZIGBEE.begin(1 << 11);
}
void loop() {
  ZIGBEE.poll();
}
