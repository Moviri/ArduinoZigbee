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
#if 0
  // If you want to explicitly set the Trust Centre Link Key enable this section
  uint8_t zb_zll_tc_key[] = {}; // insert TC_KEY as HEX if known (0xFF, 0xFF, ...)
  ZIGBEE.setTrustCenterKey(zb_zll_tc_key);
  ZIGBEE.setZigbeeRevisionLevelCompatibility(20);
#endif
  // By default all channels are used.
  ZIGBEE.begin();
}

void loop() {
  ZIGBEE.poll();
}
