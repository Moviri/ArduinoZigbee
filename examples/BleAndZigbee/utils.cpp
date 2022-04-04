#include "utils.h"

#include "Arduino.h"
#include <ArduinoZigbee.h>
#include "KVStore.h"
#include "kvstore_global_api.h"
#include "mbed_error.h"

static const char kNumLightsKey[] = "/kv/numLights";
SketchState g_sketch_state = SketchState::Bluetooth;

const char* dimmableLightColorName(unsigned int led) {
  static char* names[] = {"red", "green", "blue"};
  if ((led >= LEDR) && (led <= LEDB) ) {
    return names[led - LEDR];
  }
  return nullptr;
}

void dimmableLightPrintInfo(int led, unsigned char brightness_level) {
  Serial.print("Zigbee ");
  Serial.print(dimmableLightColorName(led));
  Serial.print(" dimmable light brightness set to ");
  Serial.print( brightness_level / 2.55 );
  Serial.println(" %");
}

unsigned int getNumLights()
{
  unsigned int num_lights = 0;
  int result = kv_get(kNumLightsKey, &num_lights, sizeof(num_lights), 0);
  switch (result)
  {
    case MBED_SUCCESS:
      if (num_lights != 0)
      {
        return num_lights;
      }
      else
      {
        Serial.println("Number of lights not configured. Please input the desired number using BLE");
        return 0;
      }
    case MBED_ERROR_ITEM_NOT_FOUND:
      Serial.println("Number of lights not configured. Please input the desired number using BLE");
      return 0;
    default:
      // Error reading from key-value store.
      Serial.println("Unknown error! Restart device");
      while (1)
        ;
  }
}

bool setNumLights(unsigned int num_lights)
{
  int result = kv_set(kNumLightsKey, &num_lights, sizeof(num_lights), 0);
  if (result != MBED_SUCCESS)
  {
    Serial.println("Unknown error! Restart device");
    return false;
  }
  return true;
}

bool isConfigured()
{
  if (!isSketchChanged())
  {
    unsigned int num_lights = getNumLights();
    if (num_lights)
    {
      Serial.print("Device is already configured: ");
      Serial.print(num_lights);
      Serial.println(" lights");
      return true;
    }
  }

  return false;
}

SketchState sketchState()
{
  return g_sketch_state;
}

void setSketchState(SketchState state)
{
  g_sketch_state = state;
}
