/*
  Demo BLE Zigbee

  This example creates a BLE peripheral with a service that contains a
  characteristic to set up how many leds will be controlled through Zigbee.
  Once the characteristic has been written to a valid value the BLE communication
  is switched off and the Zigbee communication is started.
  The number of led configured through Bluetooth is persistent across reboots.

  The circuit:
  - Arduino Nano 33 BLE board with Zigbee firmware

  You can use a generic BLE central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.

  This example code is in the public domain.
*/

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

int setupZigbee()
{
  ZigbeeDimmableLight* lights[] = {&light1, &light2, &light3};
  unsigned int num_lights = getNumLights();
  for (int i = 0; i < num_lights ; i++) {
    ZIGBEE.addEndpoint(*lights[i]);
  }
  // By default all channels are used.
  int status =  ZIGBEE.begin();
  if (status) {
    // Initialize the brightness of the configured devices.
    for (int i = 0; i < num_lights ; i++) {
      lights[i]->setBrightness(255);
    }
  }
  return status;
}
