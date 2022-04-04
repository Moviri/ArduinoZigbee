#include "Arduino.h"
#include <ArduinoBLE.h>
#include "utils.h"

// The Bluetooth service
BLEService setupService("19B10000-E8F2-537E-4F6C-D104768A1214");

// The Bluetooth characteristic used to setup the number of lights.
BLEUnsignedCharCharacteristic setupCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

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

  setNumLights(num_light);

  if (!setupZigbee()) {
    Serial.println("Starting Zigbee failed!");
    // stop here indefinitely
    while (1);
  }
  Serial.println("Starting Zigbee succeeded!");
  setSketchState(SketchState::Zigbee);
}

int setupBluetooth()
{
  // begin initialization
  int status = BLE.begin();
  if (!status) {
    return status;
  }

  // set the local name peripheral advertises
  BLE.setLocalName("SetupArduinoZigbee");
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
