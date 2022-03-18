/*
  Zigbee HA mixed endpoints example

  This example creates a zigbee device following the HA(Home Automation) profile.
  It uses the onboard temperature sensor and RGB led of an Arduino Nano 33 BLE Sense to read 
  room temperature and act as a dimmable light.

  WARNING: at the moment to use the HTS you need to modify the HTS.cpp file to ensure that
  the HTS instance is initialized as "HTS221Class HTS(Wire1);"
  To do that change the line 159 from "#ifdef ARDUINO_ARDUINO_NANO33BLE" to "#if 1".

  The circuit:
  - Arduino Nano 33 BLE Sense

  This example code is in the public domain.
*/

#include <Arduino_HTS221.h>
#include <ArduinoZigbee.h>
#include <Scheduler.h>

void dimLightCB_1(const uint8_t brightness_level) {
    analogWrite(LEDR, 255 - brightness_level);
}

float tempSensorCB_1() {
    // Put sensor read here
    return HTS.readTemperature();
}

void dimLightIdentifyCB(ZigbeeEndpoint::IdentifyEffect effect)
{
    /** Default behaviour. Can be overridden by child classes **/
    const uint32_t start_time = millis();
    switch (effect)
    {
    case ZigbeeEndpoint::IdentifyEffect::kBlink:
        /* Blink one time */
        analogWrite(LED_BUILTIN, 255);
        while (millis() - start_time < 2000)
        {
            yield();
        }
        analogWrite(LED_BUILTIN, 0);
        break;

    case ZigbeeEndpoint::IdentifyEffect::kOkay:
        analogWrite(LEDG, 0);
        while (millis() - start_time < 1000)
        {
            yield();
        }
        analogWrite(LEDG, 255);
        break;

    default:
        break;
    }
}


ZigbeeDimmableLight light("Dimmable Light v1", ZigbeeEndpoint::SourceType::kDcSource, dimLightCB_1, dimLightIdentifyCB);
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

    ZIGBEE.addEndpoint(light);
    ZIGBEE.addEndpoint(temperature_sensor);

    ZIGBEE.begin(11);
}

void loop() {
    ZIGBEE.poll();
}
