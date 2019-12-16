#ifndef MACROS_H
#define MACROS_H

// DS18B20
#define ONE_WIRE_BUS            4
#define REQUEST_SLEEP_TIME      100

// Water pump relay
#define WATER_PUMP              23

// Serial
#define SERIAL_BAUD_RATE        115200

// Blynk
#define SEND_INTERVAL           1000L
#define GETS_INTERVAL           1000L

// Delays
#define DELAY_CHECK_DATA        1000
#define DELAY_ACT               10

// Multi-core stuff
#define STACK_SIZE_FLAGS_TASKS  10000
#define NO_PARAMETERS           NULL
#define PRIORITY_1              1
#define CORE_ZERO               0
#define CORE_ONE                1

#endif