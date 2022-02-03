#include <ArduinoZigbee.h>

void dimLightCB_1(const uint8_t brightness_level) {
  analogWrite(LEDR, 255 - brightness_level);
}

void dimLightCB_2(const uint8_t brightness_level) {
  analogWrite(LEDG, 255 - brightness_level);
}

//name, endpoint id, callback
DimmableLight(dimLightCB_1);
DimmableLight(dimLightCB_2);

void setup() {
  /*
  // If you want to explicity set the TC_KEY uncomment this section

  // ZLL Commissioning Trust Centre Link Key
  #define ZB_ZLL_TC_KEY {}; // insert TC_KEY as HEX if known (FF, FF, ...)
  zb_uint8_t zb_zll_tc_key[] = ZB_ZLL_TC_KEY;
  ZIGBEE.setTrustCenterKey(zb_zll_tc_key);
  */
  ZIGBEE.begin(1 << 25);
}
void loop() {
  ZIGBEE.poll();
}
