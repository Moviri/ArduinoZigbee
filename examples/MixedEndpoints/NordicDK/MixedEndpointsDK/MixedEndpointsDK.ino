/*
  Zigbee HA mixed endpoints example

  This example creates a zigbee device following the HA(Home Automation) profile.
  It uses a simulated temperature sensor and built in led to read 
  room temperature and act as a dimmable light.

  The circuit:
  - nRF52840DK board

  This example code is in the public domain.
*/

#include <ArduinoZigbee.h>

void dimLightCB_1(const uint8_t brightness_level) {
  analogWrite(LED_BUILTIN, 255 - brightness_level);
}

float tempSensorCB_1() {
  // Put sensor read here
  return float(random(10.0,20.0)); 
}

ZigbeeDimmableLight light(dimLightCB_1);
ZigbeeTemperatureSensor temperature_sensor(tempSensorCB_1);

void setup() {
  ZIGBEE.addEndpoint(light);
  ZIGBEE.addEndpoint(temperature_sensor);
  ZIGBEE.setDeviceName("Mixed Endpoint");

  ZIGBEE.begin(11);
}

void loop() {
  ZIGBEE.poll();
}
