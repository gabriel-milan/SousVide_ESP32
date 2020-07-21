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
// Setting time
#include "time.h"
#include "sys/time.h"
// Custom macros
#include "include/SousVide.h"
// Arduino libraries
#include "OneWire.h"
#include "DallasTemperature.h"

/*
 *  Globals
 */
// Temperature
unsigned int currentTemp    = DEFAULT_VALUE;
unsigned int targetTemp     = DEFAULT_VALUE;
// Cooking time
unsigned int  currentTime   = DEFAULT_VALUE;
unsigned int  targetTime    = DEFAULT_VALUE;
// Clock
unsigned int  mClock         = DEFAULT_VALUE;
// Enable
bool enabled                = DEFAULT_VALUE;
// Control
bool heating                = false;
bool pump                   = false;
bool reachedTemperature     = false;
bool timeSet                = false;
unsigned int  startTime     = DEFAULT_VALUE;

/*
 *  Tasks
 */
// Update globals
TaskHandle_t TaskUpdateCurrent;
// Update ROM
TaskHandle_t TaskUpdateRom;
// Update BLE
TaskHandle_t TaskUpdateBleInputs;
TaskHandle_t TaskUpdateBleOutputs;
// Take actions
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
 *  Method for resetting the EEPROM
 */
void resetEeprom () {
  EEPROM_Write(EEPROM_ADDRESS_ZERO, (byte) EEPROM_INITIALIZED_VAL);
  for (int address = EEPROM_ADDRESS_ZERO + 1; address < EEPROM_SIZE; address++) {
    EEPROM_Write(address, (byte) 0);
  }
}

/*
 *  Method for writing int  into EEPROM
 */
void EEPROM_Write_Int (int address, unsigned int  val) {
  byte valBytes[4] = {0, 0, 0, 0};
  valBytes [0] = (byte) ((val >> 24) & 0xFF);
  valBytes [1] = (byte) ((val >> 16) & 0xFF);
  valBytes [2] = (byte) ((val >>  8) & 0xFF);
  valBytes [3] = (byte) ((val >>  0) & 0xFF);
  for (byte i = 0; i < sizeof(unsigned int ); i++) {
    EEPROM_Write(address + i, valBytes[i]);
  }
}

/*
 *  Method for reading int  from EEPROM
 */
unsigned int  EEPROM_Read_Int (int address) {
  unsigned int  val = 0;
  for (byte i = 0; i < sizeof(unsigned int ); i++) {
    val = ((val << 8) + (EEPROM_Read(address + i) & 0xFF));
  }
  return val;
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
 *  Method for getting current water temperature
 */
void updateCurrentTemperature () {
  sensors.requestTemperatures();
  sleep(REQUEST_SLEEP_TIME);
  float temp = sensors.getTempCByIndex(0);
  if (temp != -127) {
    currentTemp = (unsigned int) temp;
  }
  if (currentTemp >= targetTemp) {
    reachedTemperature = true;
  }
}

/*
 *  Method for updating current cook time
 */
void updateCurrentTime () {
  if ((reachedTemperature) && (timeSet)) {
    if (currentTime == DEFAULT_VALUE) {
      startTime = time(NULL);
    }
    currentTime = time(NULL) - startTime;
  }
}

/*
 *  Task for updating local variables
 */
void UpdateCurrent (void *pvParameters) {
  
  // Task loop
  for (;;) {

    // Update current temperature
    updateCurrentTemperature();

    // Update current time
    updateCurrentTime();

    // Task delay
    sleep(DELAY_TASK_UPDATE_CURR);
  }

}

/*
 *  Task for updating ROM
 */
void UpdateRom (void *pvParameters) {
  
  // Task loop
  for (;;) {

    if (currentTime != DEFAULT_VALUE) EEPROM_Write_Int (EEPROM_ADDRESS_PASSED_TIME, currentTime);
    if (targetTime  != DEFAULT_VALUE) EEPROM_Write_Int (EEPROM_ADDRESS_DESIRED_TIME, targetTime);
    if (targetTemp  != DEFAULT_VALUE) EEPROM_Write_Int (EEPROM_ADDRESS_DESIRED_TEMP, targetTemp);

    // Task delay
    sleep(DELAY_TASK_UPDATE_ROM);
  }

}

/*
 *  Function for reading characteristic value as C++ string
 */
const char *readCharacteristic (BLECharacteristic *characteristic) {
  return characteristic->getValue().c_str();
}

/*
 *  Task for updating BLE input characteristics
 */
void UpdateBleInputs (void *pvParameters) {
  
  // Task loop
  for (;;) {

    // Clock
    const char *val = readCharacteristic(pClockCharacteristic);
    if (strcmp(val, INITIAL_CHARACTERISTIC_VALUE) != 0) {
      mClock = atoi(val);
      timeval tv;
      tv.tv_sec = mClock;
      settimeofday(&tv, NULL);
      timeSet = true;
    }

    // Cooking time
    val = readCharacteristic(pCookingTimeCharacteristic);
    if (strcmp(val, INITIAL_CHARACTERISTIC_VALUE) != 0) {
      targetTime = atoi(val);
    }

    // Cooking temperature
    val = readCharacteristic(pCookingTemperatureCharacteristic);
    if (strcmp(val, INITIAL_CHARACTERISTIC_VALUE) != 0) {
      targetTemp = atoi(val);
    }

    // Enable
    val = readCharacteristic(pEnableCharacteristic);
    if (strcmp(val, INITIAL_CHARACTERISTIC_VALUE) != 0) {
      enabled = (bool) atoi(val);
    }

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

    char buffer [30];

    // Current temperature
    itoa(currentTemp, buffer, 10);
    pCurrentTemperatureCharacteristic->setValue(buffer);

    // Current temperature
    itoa(currentTime, buffer, 10);
    pTimePassedCharacteristic->setValue(buffer);

    // Current temperature
    itoa(heating, buffer, 10);
    pHeatingEnabledCharacteristic->setValue(buffer);

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

  // Initialize sensors
  sensors.begin();

  // Check if equipment has been initialized
  unsigned int firstEepromVal = EEPROM_Read(EEPROM_ADDRESS_ZERO);
  if (firstEepromVal != EEPROM_INITIALIZED_VAL) {
    log ("-> Initializing EEPROM...");
    resetEeprom();
  }

  // Initializing Bluetooth
  initBle();

  // Pin TaskUpdateCurrent to Core #0
  xTaskCreatePinnedToCore(
    UpdateCurrent,              /* Task function. */
    "TaskUpdateCurrent",        /* Name of task. */
    STACK_SIZE_DEFAULT,         /* Stack size of task */
    NO_PARAMETERS,              /* Parameters of the task */
    PRIORITY_DEFAULT,           /* Priority of the task (the higher, the more important) */
    &TaskUpdateCurrent,         /* Task handle to keep track of created task */
    CORE_ZERO                   /* Pin task to core 1 */
  );
  sleep(500);

  // Pin TaskUpdateRom to Core #1
  xTaskCreatePinnedToCore(
    UpdateRom,                  /* Task function. */
    "TaskUpdateRom",            /* Name of task. */
    STACK_SIZE_DEFAULT,         /* Stack size of task */
    NO_PARAMETERS,              /* Parameters of the task */
    PRIORITY_DEFAULT,           /* Priority of the task (the higher, the more important) */
    &TaskUpdateRom,             /* Task handle to keep track of created task */
    CORE_ONE                    /* Pin task to core 1 */
  );
  sleep(500);

  // Pin TaskUpdateBleInputs to Core #1
  xTaskCreatePinnedToCore(
    UpdateBleInputs,            /* Task function. */
    "TaskUpdateBleInputs",      /* Name of task. */
    STACK_SIZE_DEFAULT,         /* Stack size of task */
    NO_PARAMETERS,              /* Parameters of the task */
    PRIORITY_DEFAULT,           /* Priority of the task (the higher, the more important) */
    &TaskUpdateBleInputs,       /* Task handle to keep track of created task */
    CORE_ONE                    /* Pin task to core 1 */
  );
  sleep(500);

  // Pin TaskUpdateBleOutputs to Core #0
  xTaskCreatePinnedToCore(
    UpdateBleOutputs,           /* Task function. */
    "TaskUpdateBleOutputs",     /* Name of task. */
    STACK_SIZE_DEFAULT,         /* Stack size of task */
    NO_PARAMETERS,              /* Parameters of the task */
    PRIORITY_DEFAULT,           /* Priority of the task (the higher, the more important) */
    &TaskUpdateBleOutputs,      /* Task handle to keep track of created task */
    CORE_ZERO                   /* Pin task to core 1 */
  );
  sleep(500);

  // Pin TaskActions to Core #0
  xTaskCreatePinnedToCore(
    Actions,                    /* Task function. */
    "TaskActions",              /* Name of task. */
    STACK_SIZE_DEFAULT,         /* Stack size of task */
    NO_PARAMETERS,              /* Parameters of the task */
    PRIORITY_DEFAULT,           /* Priority of the task (the higher, the more important) */
    &TaskActions,               /* Task handle to keep track of created task */
    CORE_ZERO                   /* Pin task to core 1 */
  );
  sleep(500);
}