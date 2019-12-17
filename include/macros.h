#ifndef MACROS_H
#define MACROS_H

// Pinout
#define ONE_WIRE_BUS            4
#define WATER_HEATING           22
#define WATER_PUMP              23

// Serial
#define SERIAL_BAUD_RATE        115200

// Delays
#define DELAY_UPDATE_LOCAL      10
#define DELAY_UPDATE_BLYNK      10
#define DELAY_ACTIONS           10
#define REQUEST_SLEEP_TIME      100

// Multi-core stuff
#define STACK_SIZE_FLAGS_TASKS  10000
#define NO_PARAMETERS           NULL
#define PRIORITY_1              1
#define CORE_ZERO               0
#define CORE_ONE                1

#endif