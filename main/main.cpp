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
#include "nvs_flash.h"
// Custom macros
#include "include/SousVide.h"
// Arduino libraries
#include "OneWire.h"
#include "DallasTemperature.h"

/*
 *  Globals
 */
float currentTemp = -127;

/*
 *  Tasks
 */
TaskHandle_t TaskUpdateCurrentTemp;
TaskHandle_t TaskUpdatePassedTime;
TaskHandle_t TaskUpdateBleInputs;
TaskHandle_t TaskUpdateBleOutputs;
TaskHandle_t TaskActions;

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
BLECharacteristic *pClockCharacteristic;
BLECharacteristic *pCookingTimeCharacteristic;
BLECharacteristic *pCookingTemperatureCharacteristic;
BLECharacteristic *pEnableCharacteristic;
// Outputs
BLECharacteristic *pCurrentTemperatureCharacteristic;
BLECharacteristic *pTimePassedCharacteristic;
BLECharacteristic *pHeatingEnabledCharacteristic;
// Advertising
BLEAdvertising *pAdvertising;

/*
 *  Logging function
 */
void log (const char *message, const char *end="\n") {
  if (DEBUG_MODE) printf ("%s%s", message, end);
}

/*
 *  Method that sleeps using the vTaskDelay function
 */
void sleep (int ms) {
  vTaskDelay(pdMS_TO_TICKS(ms));
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
void initBle () {

  // BLE Server
  log("-> Starting BLE stuff...");
  BLEDevice::init(DEVICE_NAME);
  pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);

  // Inputs
  log("  * Creating characteristic for clock...");
  pClockCharacteristic = pService->createCharacteristic(CLOCK_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  pClockCharacteristic->setValue(INITIAL_CHARACTERISTIC_VALUE);

  log("  * Creating characteristic for cooking time...");
  pCookingTimeCharacteristic = pService->createCharacteristic(COOKING_TIME_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  pCookingTimeCharacteristic->setValue(INITIAL_CHARACTERISTIC_VALUE);

  log("  * Creating characteristic for cooking temperature...");
  pCookingTemperatureCharacteristic = pService->createCharacteristic(COOKING_TEMP_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  pCookingTemperatureCharacteristic->setValue(INITIAL_CHARACTERISTIC_VALUE);

  log("  * Creating characteristic for enabling SousVide...");
  pEnableCharacteristic = pService->createCharacteristic(ENABLE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  pEnableCharacteristic->setValue(INITIAL_CHARACTERISTIC_VALUE);

  // Outputs
  log("  * Creating characteristic for current temperature...");
  pCurrentTemperatureCharacteristic = pService->createCharacteristic(CURRENT_TEMP_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
  pCurrentTemperatureCharacteristic->setValue(INITIAL_CHARACTERISTIC_VALUE);

  log("  * Creating characteristic for current temperature...");
  pTimePassedCharacteristic = pService->createCharacteristic(TIME_PASSED_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
  pTimePassedCharacteristic->setValue(INITIAL_CHARACTERISTIC_VALUE);

  log("  * Creating characteristic for current temperature...");
  pHeatingEnabledCharacteristic = pService->createCharacteristic(HEATING_EN_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
  pHeatingEnabledCharacteristic->setValue(INITIAL_CHARACTERISTIC_VALUE);

  // Advertising
  log("  * Starting advertising...");
  pService->start();
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

	log("  [OK] BLE setup done!");
}

/*
 *  Method for resetting the EEPROM
 */
void resetEeprom () {
  EEPROM_Write(EEPROM_ADDRESS_ZERO, (byte) EEPROM_INITIALIZED_VAL);
  for (int address = EEPROM_ADDRESS_ZERO + 1; address < EEPROM_SIZE; address++) {
    EEPROM_Write(address, (byte) 0);
  }
}

/*
 *  Method for getting current water temperature
 */
void updateCurrentTemperature () {
  sensors.requestTemperatures();
  sleep(REQUEST_SLEEP_TIME);
  float temp = sensors.getTempCByIndex(0);
  if (temp != -127) {
    currentTemp = temp;
  }
}

/*
 *  Task for updating current temperature (local variable)
 */
void UpdateCurrentTemp (void *pvParameters) {
  
  // Task loop
  for (;;) {

    // Update method
    updateCurrentTemperature();

    // Task delay
    sleep(DELAY_TASK_UPDATE_CURR_TEMP);
  }

}

/*
 *  Task for updating passed time (ROM)
 */
void UpdatePassedTime (void *pvParameters) {
  
  // Task loop
  for (;;) {

    // TODO: Implement this

    // Task delay
    sleep(DELAY_TASK_UPDATE_PASSED_TIME);
  }

}

/*
 *  Task for updating BLE input characteristics
 */
void UpdateBleInputs (void *pvParameters) {
  
  // Task loop
  for (;;) {

    // TODO: Implement this

    // Task delay
    sleep(DELAY_TASK_UPDATE_BLE_INPUTS);
  }

}

/*
 *  Task for updating BLE output characteristics
 */
void UpdateBleOutputs (void *pvParameters) {
  
  // Task loop
  for (;;) {

    // TODO: Implement this

    // Task delay
    sleep(DELAY_TASK_UPDATE_BLE_OUTPUTS);
  }

}

/*
 *  Task for performing actions on the heater
 */
void Actions (void *pvParameters) {
  
  // Task loop
  for (;;) {

    // TODO: Implement this

    // Task delay
    sleep(DELAY_TASK_ACTIONS);
  }

}

/*
 *  Main function
 */
extern "C" void app_main (void) {

  // Initializing NVS
  nvs_flash_init();

  // Setting pin modes
  pinMode(WATER_PUMP, OUTPUT);
  pinMode(WATER_HEATING, OUTPUT);

  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);

  // Check if equipment has been initialized
  unsigned int firstEepromVal = EEPROM_Read(EEPROM_ADDRESS_ZERO);
  if (firstEepromVal != EEPROM_INITIALIZED_VAL) {
    log ("-> Initializing EEPROM...");
    resetEeprom();
  }

  // Pin TaskUpdateCurrentTemp to Core #0
  xTaskCreatePinnedToCore(
                    UpdateCurrentTemp,          /* Task function. */
                    "TaskUpdateCurrentTemp",    /* Name of task. */
                    STACK_SIZE_DEFAULT,         /* Stack size of task */
                    NO_PARAMETERS,              /* Parameters of the task */
                    PRIORITY_DEFAULT,           /* Priority of the task (the higher, the more important) */
                    &TaskUpdateCurrentTemp,     /* Task handle to keep track of created task */
                    CORE_ZERO);                 /* Pin task to core 1 */
  sleep(500);

}