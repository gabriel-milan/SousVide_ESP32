/*
 * DIY SousVide implementation
 * Author: Gabriel Gazola Milan
 * Git: https://github.com/gabriel-milan/SousVide_ESP32
 * 
 * Disclaimer: Work in progress!
 */

/*
 *  Includes
 */
#include <stdio.h>
// For using Arduino libraries
#include "Arduino.h"
// BLE stuff
#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
// EEPROM
#include "EEPROM.h"
// ESP32 stuff
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
// Custom macros
#include "include/SousVide.h"
// Arduino libraries
#include "OneWire.h"
#include "DallasTemperature.h"

/*
 *  DS18B20 declaration
 */
OneWire oneWire (ONE_WIRE_BUS);
DallasTemperature sensors (&oneWire);

/*
 *  BLE stuff
 */
BLEServer *pServer;
BLEService *pService;
// Inputs
BLECharacteristic *pCookingTimeCharacteristic;
BLECharacteristic *pCookingTemperatureCharacteristic;
BLECharacteristic *pEnableCharacteristic;
// Outputs
BLECharacteristic *pTimePassedCharacteristic;
BLECharacteristic *pHeatingEnabledCharacteristic;

/*
 *  Logging function
 */
void log (const char *message, const char *end="\n") {
  if (DEBUG_MODE) printf ("%s%s", message, end);
}

/*
 *  Method that writes on EEPROM
 */
void EEPROM_Write (int address, byte value) {
  EEPROM.write(address, value);
  EEPROM.commit();
}

/*
 *  Method that reads from EEPROM
 */
byte EEPROM_Read (int address) {
  return EEPROM.read(address);
}

/*
 *  Method that begins BLE server, service and characteristic
 */
void BeginBleServer (bool installMode) {

  log("--- Starting BLE stuff...");
  // BLEDevice::init(DEVICE_NAME);
  // pServer = BLEDevice::createServer();
  // pService = pServer->createService(SERVICE_UUID);

  // if (installMode) {
  //   if (DEBUG_MODE)
  //     Serial.println("Creating characteristic for installation...");
  //   pInstallationCharacteristic = pService->createCharacteristic(INSTALLATION_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  //   pInstallationCharacteristic->setValue(INSTALLATION_CHARACTERISTIC_VALUE);
  // }

  // if (DEBUG_MODE)
  //   Serial.println("Creating characteristic for reading...");
  // pFraudCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
  // pFraudCharacteristic->setValue(INITIAL_CHARACTERISTIC_VALUE);

  // if (DEBUG_MODE)
  //   Serial.println("Creating characteristic for resetting...");
  // pResetCharacteristic = pService->createCharacteristic(RESET_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  // pResetCharacteristic->setValue(INSTALLATION_CHARACTERISTIC_VALUE);

  // if (DEBUG_MODE)
  //   Serial.println("Starting advertising...");
  // pService->start();
  // pAdvertising = BLEDevice::getAdvertising();
  // pAdvertising->addServiceUUID(SERVICE_UUID);
  // pAdvertising->setScanResponse(true);
  // pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  // pAdvertising->setMinPreferred(0x12);
  // BLEDevice::startAdvertising();

	// if (DEBUG_MODE)
  // 	Serial.println("BLE setup done!");
}

/*
 *  Main function
 */
extern "C" void app_main (void) {
}