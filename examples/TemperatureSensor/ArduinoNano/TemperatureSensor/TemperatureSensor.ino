/*
  Zigbee HA Temperature Sensor example

  This example creates a zigbee device following the HA(Home Automation) profile.
  It uses the onboard temperature sensor of an Arduino Nano 33 BLE Sense to read 
  room temperature
  
  WARNING: at the moment to use the HTS you need to modify the HTS.cpp file to ensure that
  the HTS instance is initialized as "HTS221Class HTS(Wire1);"
  To do that change the line 159 from "#ifdef ARDUINO_ARDUINO_NANO33BLE" to "#if 1".

  The circuit:
  - Arduino Nano 33 BLE Sense

  This example code is in the public domain.
*/

#include <Arduino_HTS221.h>
#include <ArduinoZigbee.h>

float tempSensorCB_1() {
  // Put sensor read here
  return HTS.readTemperature(); 
}

ZigbeeTemperatureSensor temperature_sensor(tempSensorCB_1);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started!");

  if (!HTS.begin()) {
    Serial.println("Failed to initialize humidity temperature sensor!");
    while (1);
  }
  Serial.println("Sensors initialized!");

  ZIGBEE.addEndpoint(temperature_sensor);

  ZIGBEE.begin(11);
}

void loop() {
  ZIGBEE.poll();
}
