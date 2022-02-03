/*
  Zigbee HA Dimmable Light example

  This example creates a zigbee device following the HA(Home Automation) profile.
  It uses the onboard RGB led of an Arduino Nano 33 BLE Sense to simulate to
  dimmable lights.

  The circuit:
  - Arduino Nano 33 BLE Sense

  This example code is in the public domain.
*/

#include <ArduinoZigbee.h>

void dimLightCB_1(const uint8_t brightness_level) {
  analogWrite(LEDR, 255 - brightness_level);
}

void dimLightCB_2(const uint8_t brightness_level) {
  analogWrite(LEDG, 255 - brightness_level);
}

DimmableLight(dimLightCB_1);
DimmableLight(dimLightCB_2);

void setup() {
  /*
  // If you want to explicity set the TC_KEY uncomment this section

  // ZLL Commissioning Trust Centre Link Key
  #define ZB_ZLL_TC_KEY {}; // insert TC_KEY as HEX if known (0xFF, 0xFF, ...)
  zb_uint8_t zb_zll_tc_key[] = ZB_ZLL_TC_KEY;
  ZIGBEE.setTrustCenterKey(zb_zll_tc_key);
  */
  ZIGBEE.begin(1 << 11);
}
void loop() {
  ZIGBEE.poll();
}
