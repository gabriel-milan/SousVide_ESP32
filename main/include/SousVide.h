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
#define CHARACTERISTIC_UUID               "68b44d72-e082-4581-bc29-d4c5ed732476"
#define INITIAL_CHARACTERISTIC_VALUE      "N/A"
#define INSTALLATION_CHARACTERISTIC_VALUE "0000"
// MultiCore stuff
#define CORE_ZERO                         0
#define CORE_ONE                          1
#define PRIORITY_FLAGS_TASKS              1
#define STACK_SIZE_FLAGS_TASKS            10000
#define NO_PARAMETERS                     NULL
// Tasks delays
#define DELAY_TASK_DEFAULT                10
#define DELAY_TASK_UPDATE_LOCAL           DELAY_TASK_DEFAULT
#define DELAY_TASK_ACTIONS                DELAY_TASK_DEFAULT
// EEPROM stuff
#define EEPROM_SIZE                       512

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