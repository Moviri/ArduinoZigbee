/*
  Zigbee HA Temperature Sensor example

  This example creates a zigbee device following the HA(Home Automation) profile.
  It uses a simulated temperature sensor

  The circuit:
  - nRF52840DK board

  This example code is in the public domain.
*/
#include <ArduinoZigbee.h>

float tempSensorCB() {
  // Put sensor read here
  return float(random(20.0,28.0)); 
}

ZigbeeTemperatureSensor temperature_sensor(tempSensorCB);

void setup() {
  ZIGBEE.addEndpoint(temperature_sensor);
  
  // By default all channels are used.
  ZIGBEE.begin();
}

void loop() {
  ZIGBEE.poll();
}
