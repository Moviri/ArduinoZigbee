
#include <ArduinoZigbee.h>
#include <ArduinoBLE.h>
#include "utils.h"

void dimLightCB_1(const uint8_t brightness_level) {
  analogWrite(LEDR, 255 - brightness_level);
  dimmableLightPrintInfo(LEDR, brightness_level);
}

void dimLightCB_2(const uint8_t brightness_level) {
  analogWrite(LEDG, 255 - brightness_level);
  dimmableLightPrintInfo(LEDG, brightness_level);
}

void dimLightCB_3(const uint8_t brightness_level) {
  analogWrite(LEDB, 255 - brightness_level);
  dimmableLightPrintInfo(LEDB, brightness_level);
}

ZigbeeDimmableLight light1(dimLightCB_1);
ZigbeeDimmableLight light2(dimLightCB_2);
ZigbeeDimmableLight light3(dimLightCB_3);

int setupZigbee()
{
  ZigbeeDimmableLight* lights[] = {&light1, &light2, &light3};
  unsigned int num_lights = getNumLights();
  for (int i = 0; i < num_lights ; i++) {
    ZIGBEE.addEndpoint(*lights[i]);
  }
  // By default all channels are used.
  return ZIGBEE.begin();
}

void setup() {
  Serial.begin(9600);
  // Wait 30 seconds for the serial to be available
  while ((!Serial) && (millis() < 30000));

  if (isConfigured()) {
    if (!setupZigbee()) {
      Serial.println("Starting Zigbee failed!");
      // stop here indefinitely
      while (1);
    }
    Serial.println("Starting Zigbee succeeded!");
    setSketchState(SketchState::Zigbee);
  }
  else {
    if (!setupBluetooth()) {
      Serial.println("Starting Bluetooth failed!");
      // stop here indefinitely
      while (1);
    }
    Serial.println("Bluetooth device active, waiting for connections...");
    setSketchState(SketchState::Bluetooth);
  }
}

void loop() {
  switch (sketchState()) {
    case SketchState::Bluetooth:
      BLE.poll();
      break;
    case SketchState::Zigbee:
      ZIGBEE.poll();
      break;
  }
}
