/*
  Debo BLE Zigbee

  This example creates a BLE peripheral with service that contains a
  characteristic to set up how many leds will be controlled through Zigbee.
  Once the characteristic has been written to a valid value the BLE communication
  is switched off and the Zigbee communication is started.

  The circuit:
  - Arduino Nano 33 BLE board with Zigbee firmware

  You can use a generic BLE central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>
#include <ArduinoZigbee.h>

#include "KVStore.h"
#include "kvstore_global_api.h"
#include "mbed_error.h"

BLEService setupService("19B10000-E8F2-537E-4F6C-D104768A1214"); // create service

// create switch characteristic and allow remote device to read and write
BLEUnsignedCharCharacteristic setupCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

enum class DeviceState {
  BleInit = 0,
  BleRunning = 1,
  ZigbeeInit = 2,
  ZigbeeRunning = 3
};

static const char kNumLightsKey[] = "/kv/numLights";

unsigned int g_num_light = 0;
DeviceState g_state = DeviceState::BleInit;
bool g_skipBLE = false;

void setDeviceState(DeviceState state) {
  g_state = state;
}

DeviceState deviceState() {
  return g_state;
}

void dimLightCB_1(const uint8_t brightness_level) {
  analogWrite(LEDR, 255 - brightness_level);
}

void dimLightCB_2(const uint8_t brightness_level) {
  analogWrite(LEDG, 255 - brightness_level);
}

void dimLightCB_3(const uint8_t brightness_level) {
  analogWrite(LEDB, 255 - brightness_level);
}

ZigbeeDimmableLight light1(dimLightCB_1);
ZigbeeDimmableLight light2(dimLightCB_2);
ZigbeeDimmableLight light3(dimLightCB_3);

int setupZigbee() {
  ZigbeeDimmableLight* lights[] = {&light1, &light2, &light3};
  for (int i = 0; i < g_num_light ; i++) {
    ZIGBEE.addEndpoint(*lights[i]);
  }
  // By default all channels are used.
  return ZIGBEE.begin();
}

void blePeripheralConnectHandler(BLEDevice central) {
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  // central disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}

void setupCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  unsigned int num_light = setupCharacteristic.value();
  if (num_light <= 0 || num_light > 3) {
    Serial.print("Invalid quantity: ");
    Serial.println(num_light);
    Serial.println("Please write a number between 1 and 3");
    return;
  }

  Serial.print(num_light);
  Serial.println(" Zigbee lights will be configured");

  g_num_light = num_light;
  int result = kv_set(kNumLightsKey, &g_num_light, sizeof(g_num_light), 0);
  if (result != MBED_SUCCESS) {
    Serial.println("Unknown error! Restart device");
  }

  // Change mode to Zigbee
  setDeviceState(DeviceState::ZigbeeInit);
}

int setupBle() {
  // begin initialization
  int status = BLE.begin();
  if (!status) {
    return status;
  }

  // set the local name peripheral advertises
  BLE.setLocalName("ZigbeeSetupSense");
  // set the UUID for the service this peripheral advertises
  BLE.setAdvertisedService(setupService);

  // add the characteristic to the service
  setupService.addCharacteristic(setupCharacteristic);

  // add service
  BLE.addService(setupService);

  // assign event handlers for connected, disconnected to peripheral
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  // assign event handlers for characteristic
  setupCharacteristic.setEventHandler(BLEWritten, setupCharacteristicWritten);
  // set an initial value for the characteristic
  setupCharacteristic.setValue(0);

  // start advertising
  BLE.advertise();
  return 1;
}

unsigned int readNumLights() {
  unsigned int num_lights = 0;

  int result = kv_get(kNumLightsKey, &num_lights, sizeof(num_lights), 0);

  switch (result)
  {
    case MBED_SUCCESS:
      if (num_lights != 0) {
        Serial.println("NVRAM read successfully");
        return num_lights;
      }
      else {
        Serial.println("Number of lights not configured. Please input the desired number using BLE");
        return 0;
      }
      break;
    case MBED_ERROR_ITEM_NOT_FOUND:
      Serial.println("Number of lights not configured. Please input the desired number using BLE");
      return 0;
      break;
    default:
      // Error reading from key-value store.
      Serial.println("Unknown error! Restart device");
      while (1);
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  delay(1000);

  if (!isSketchChanged()) {
    g_num_light = readNumLights();
    if (g_num_light) {
      setDeviceState(DeviceState::ZigbeeInit);
      g_skipBLE = true;

      Serial.print("Device is already configured: ");
      Serial.println(g_num_light);
    }
  }
  if (!g_skipBLE) {
    if (!setupBle()) {
      Serial.println("starting BLE failed!");
      // stop here indefinitely
      while (1);
    }
    
    int result = kv_set(kNumLightsKey, &g_num_light, sizeof(g_num_light), 0);
    if (result != MBED_SUCCESS) {
      Serial.println("Unknown error! Restart device");
    }

    setDeviceState(DeviceState::BleRunning);
    Serial.println("Bluetooth device active, waiting for connections...");
  }
}

void loop() {
  switch (deviceState()) {
    case DeviceState::BleInit:
      break;
    case DeviceState::BleRunning:
      // poll for BLE events
      BLE.poll();
      break;
    case DeviceState::ZigbeeInit:
      if (!g_skipBLE) {
        BLE.end();
      }
      
      if (!setupZigbee()) {
        Serial.println("starting Zigbee failed!");
        // stop here indefinitely
        while (1);
      }
      Serial.println("starting Zigbee succeeded!");
      setDeviceState(DeviceState::ZigbeeRunning);
      break;
    case DeviceState::ZigbeeRunning:
      ZIGBEE.poll();
      break;
  }
}
