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

DimmableLight(dimLightCB_1);
TemperatureSensor(tempSensorCB_1);

void setup() {
  ZIGBEE.begin(1 << 11);
}
void loop() {
  ZIGBEE.poll();
}
