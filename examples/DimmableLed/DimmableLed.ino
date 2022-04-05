/*
  Zigbee HA Dimmable Light example

  This example creates a zigbee device following the HA(Home Automation) profile.
  It uses the onboard RGB led of an Arduino Nano 33 BLE Sense to simulate two
  dimmable lights.

  The circuit:
  - Arduino Nano 33 BLE Sense

  This example code is in the public domain.
*/

#include <ArduinoZigbee.h>

void dimLightCB(const uint8_t brightness_level) {
  analogWrite(LEDR, 255 - brightness_level);
  Serial.print("Dimmable light brightness: ");
  Serial.print((int)(brightness_level/2.550));
  Serial.println(" %");
}

ZigbeeDimmableLight light(dimLightCB);

void setup() {
  Serial.begin(9600);
  // Wait 30 seconds for the serial to be available
  while ((!Serial) && (millis() < 30000));

  ZIGBEE.addEndpoint(light);
  // By default all channels are used.
  ZIGBEE.begin();
}

void loop() {
  ZIGBEE.poll();
}