/*
  Zigbee HA Dimmable Light example

  This example creates a zigbee device following the HA(Home Automation) profile.
  It uses the onboard led of an nRF52840DK board to simulate a
  dimmable lights.

  The circuit:
  - nRF52840DK board

  This example code is in the public domain.
*/

#include <ArduinoZigbee.h>

void dimLightCB_1(const uint8_t brightness_level) {
  analogWrite(LED_BUILTIN, 255 - brightness_level);
}

ZigbeeDimmableLight light(dimLightCB_1);

void setup() {
  ZIGBEE.addEndpoint(light);
  /*
  // If you want to explicitly set the TC_KEY uncomment this section

  // ZLL Commissioning Trust Centre Link Key
  #define ZB_ZLL_TC_KEY {}; // insert TC_KEY as HEX if known (0xFF, 0xFF, ...)
  zb_uint8_t zb_zll_tc_key[] = ZB_ZLL_TC_KEY;
  ZIGBEE.setTrustCenterKey(zb_zll_tc_key);
  */
  ZIGBEE.begin(11);
}

void loop() {
  ZIGBEE.poll();
}
