#include <ArduinoZigbee.h>

void dimLightCB_1(const uint8_t brightness_level) {
  analogWrite(LEDR, 255 - brightness_level);
}

void dimLightCB_2(const uint8_t brightness_level) {
  analogWrite(LEDG, 255 - brightness_level);
}

#define EP_1 12
#define EP_2 13

//name, endpoint id, callback
DimmableLight(light_1, EP_1, dimLightCB_1)
DimmableLight(light_2, EP_2, dimLightCB_2)

void setup() {
  ZIGBEE.begin(1<<25);
}
void loop() {
  ZIGBEE.poll();
}