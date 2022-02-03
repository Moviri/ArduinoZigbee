/*
  Zigbee HA mixed endpoints example

  This example creates a zigbee device following the HA(Home Automation) profile.
  It uses the onboard temperature sensor and RGB led of an Arduino Nano 33 BLE Sense to read 
  room temperature and act as a dimmable light.

  The circuit:
  - Arduino Nano 33 BLE Sense

  This example code is in the public domain.
*/

#include <Arduino_HTS221.h>
#include <ArduinoZigbee.h>

void dimLightCB_1(const uint8_t brightness_level) {
    analogWrite(LEDR, 255 - brightness_level);
}

float tempSensorCB_1() {
    // Put sensor read here
    return HTS.readTemperature();
}

DimmableLight(dimLightCB_1);
TemperatureSensor(tempSensorCB_1);

void setup() {
    Serial.begin(9600);
    while (!Serial);
    Serial.println("Started!");

    if (!HTS.begin()) {
        Serial.println("Failed to initialize humidity temperature sensor!");
        while (1);
    }
    Serial.println("Sensors initialized!");

    ZIGBEE.begin(1 << 11);
}
void loop() {
    ZIGBEE.poll();
}
