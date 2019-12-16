/*
 *  Includes
 */
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BlynkSimpleEsp32_BLE.h>
#include "credentials.h"
#include "macros.h"

/*
 *  DS18B20 declaration
 */
OneWire oneWire (ONE_WIRE_BUS);
DallasTemperature sensors (&oneWire);

/*
 *  Globals
 */
unsigned int  targetTemp    = 0;
unsigned int  currentTemp   = 0;
long          startTime     = 0;
long          offsetTime    = 0;
long          endingTime    = 0;

/*
 *  States
 */
bool          enable        = false;
bool          heating       = false;
bool          waterPump     = false;
bool          timeIsUp      = false;

/*
 *  Tasks
 */
TaskHandle_t TaskReadingData;
TaskHandle_t TaskAct;

/*
 *  Method that sleeps using the vTaskDelay function
 */
void sleep (int ms) {
  vTaskDelay(pdMS_TO_TICKS(ms));
}

/*
 *  Function that returns the value of the temperature in Celsius
 */
void getTempCelsius () {

  sensors.requestTemperatures();
  sleep(REQUEST_SLEEP_TIME);
  float temp = sensors.getTempCByIndex(0);
  if (temp != -127) {
    currentTemp = (unsigned int) temp;
  }

}

/*
 * Method that updates if heating should be enabled
 */
void updateHeating () {
  if (currentTemp < targetTemp) {
    heating = true;
  }
  else {
    heating = false;
  }
}

/*
 *  Blynk inputs
 */
// V2 - Time input that sends the desired cooking time
BLYNK_WRITE(V2) {
  TimeInputParam t(param);

  if (t.hasStartTime()) {
    offsetTime = 
      t.getStartHour() * 3600 + 
      t.getStartMinute() * 60 +
      t.getStartSecond();
    offsetTime *= 1000;
  }

  endingTime = startTime + offsetTime;
}
// V3 - Numeric input that sends the target temperature
BLYNK_WRITE(V3) {
  targetTemp = param.asInt();
}
// V4 - Styled button that enables or disables the sous vide
BLYNK_WRITE(V4) {
  enable = param.asInt();
}

/*
 *  Method for reading data
 */
void ReadingData (void *pvParameters) {

  // Loop
  for (;;) {

    // Updating temperature
    getTempCelsius();

    // Update heating
    updateHeating();

    // Task delay
    sleep (DELAY_CHECK_DATA);
  }
}

/*
 *  Method for acting
 */
void Act (void *pvParameters) {

  // Loop
  for (;;) {

    // Sets water pump
    digitalWrite(WATER_PUMP, enable);

    // Sets the Blynk LED
    Blynk.virtualWrite(1, heating);

    // Task delay
    sleep (DELAY_ACT);
  }
}

/*
 *  Setup
 */
void setup() {

  // Initializing serial connection
  Serial.begin(SERIAL_BAUD_RATE);

  // Initializing the sensor
  sensors.begin();

  // Initialize Blynk
  Blynk.setDeviceName("TEST ME");
  Blynk.begin(token);

  // Initialize pins
  pinMode(WATER_PUMP, OUTPUT);

  xTaskCreatePinnedToCore(
                    ReadingData,       /* Task function. */
                    "TaskReadingData", /* Name of task. */
                    STACK_SIZE_FLAGS_TASKS,     /* Stack size of task */
                    NO_PARAMETERS,              /* Parameters of the task */
                    PRIORITY_1,       /* Priority of the task (the higher, the more important) */
                    &TaskReadingData,  /* Task handle to keep track of created task */
                    CORE_ZERO);                 /* Pin task to core 1 */
  sleep(500);

  xTaskCreatePinnedToCore(
                    Act,       /* Task function. */
                    "TaskAct", /* Name of task. */
                    STACK_SIZE_FLAGS_TASKS,     /* Stack size of task */
                    NO_PARAMETERS,              /* Parameters of the task */
                    PRIORITY_1,       /* Priority of the task (the higher, the more important) */
                    &TaskAct,  /* Task handle to keep track of created task */
                    CORE_ZERO);                 /* Pin task to core 1 */
  sleep(500);

}

/*
 *  Loop
 */
void loop() {

  Blynk.run();

  // Serial.print("Current temp: ");
  // Serial.print(currentTemp);
  // Serial.print("ºC / Target temp: ");
  // Serial.print(targetTemp);
  // Serial.print("ºC / Heating: ");
  // Serial.print(heating);
  // Serial.print(" / Water pump: ");
  // Serial.print(enable);
  // Serial.print(" / Millis: ");
  // Serial.print(millis());
  // Serial.print(" / EndingTime: ");
  // Serial.println(endingTime);

}