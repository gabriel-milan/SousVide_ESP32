#ifndef SOUSVIDE_H
#define SOUSVIDE_H

/*
 *  Macros
 */
// Serial stuff
#define DEBUG_MODE                        true
#define SERIAL_BAUD_RATE                  115200
// BLE stuff
#define DEVICE_NAME                       "SousVide"
#define SERVICE_UUID                      "90f72f6e-6e9a-459d-b21b-8a2d91039b52"
#define CLOCK_CHARACTERISTIC_UUID         "8866e27c-2361-4f92-83e4-cdd68e99291c"
#define COOKING_TIME_CHARACTERISTIC_UUID  "68b44d72-e082-4581-bc29-d4c5ed732476"
#define COOKING_TEMP_CHARACTERISTIC_UUID  "e4c3be56-fb34-49b6-82c5-b8cba058c223"
#define ENABLE_CHARACTERISTIC_UUID        "cd5399c9-05a3-473a-8464-3a64b54bc46f"
#define CURRENT_TEMP_CHARACTERISTIC_UUID  "c03c549b-82b7-4bd7-9730-20934a09d5a0"
#define TIME_PASSED_CHARACTERISTIC_UUID   "7f27daa0-f648-43c7-9dbc-0a81bcc83ff4"
#define HEATING_EN_CHARACTERISTIC_UUID    "ad4d945b-9a6e-4792-8bf4-f7097352c9c8"
#define INITIAL_CHARACTERISTIC_VALUE      "Unknown"
// MultiCore stuff
#define CORE_ZERO                         0
#define CORE_ONE                          1
#define PRIORITY_DEFAULT                  1
#define STACK_SIZE_DEFAULT                10000
#define NO_PARAMETERS                     NULL
// Tasks delays
#define DELAY_TASK_DEFAULT                10
#define DELAY_TASK_UPDATE_CURR            500                 // Core #0
#define DELAY_TASK_UPDATE_ROM             5000                // Core #1
#define DELAY_TASK_UPDATE_PASSED_TIME     5000                // Core #1
#define DELAY_TASK_UPDATE_BLE_INPUTS      5000                // Core #1
#define DELAY_TASK_UPDATE_BLE_OUTPUTS     1000                // Core #0
#define DELAY_TASK_ACTIONS                DELAY_TASK_DEFAULT  // Core #0
// EEPROM stuff
#define EEPROM_SIZE                       512
#define EEPROM_INITIALIZED_VAL            43 // Arbitrary value
#define EEPROM_ADDRESS_ZERO               0
#define EEPROM_ADDRESS_TIMESTAMP_START    1
#define EEPROM_ADDRESS_PASSED_TIME        5
#define EEPROM_ADDRESS_DESIRED_TIME       9
#define EEPROM_ADDRESS_DESIRED_TEMP       13
#define EEPROM_ADDRESS_NEXT               17
// Defaults
#define DEFAULT_VALUE                     0
// Temperature sensor
#define REQUEST_SLEEP_TIME                100

/*
 *  Pinout
 */
// OneWire
#define ONE_WIRE_BUS                      4
// Water heating
#define WATER_HEATING                     22
// Water pump
#define WATER_PUMP                        23

#endif