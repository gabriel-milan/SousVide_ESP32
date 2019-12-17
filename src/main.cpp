/*
 * DIY SousVide implementation
 * Author: Gabriel Gazola Milan
 * Git: https://github.com/gabriel-milan/SousVide
 * 
 * Inputs:
 * - DS18B20 temperature sensor (OneWire connection on Pin 4)
 * - Cooking time input (Virtual Pin 2)
 * - Desired cooking temperature (Virtual Pin 3)
 * - On/Off switch (Virtual Pin 4)
 * 
 * Outputs:
 * - Water heating (Digital Pin 22)
 * - Water pump (Digital Pin 23)
 * - LCD (Virtual Pin V0)
 * - Heating LED (Virtual Pin V1)
 * 
 * Tasks:
 * - TaskUpdateLocal (Core one)  -> Updates local inputs and globals
 * - TaskUpdateBlynk (Core zero) -> Updates Blynk app outputs (LCD / LED)
 * - TaskActions     (Core zero) -> Acts on the local outputs (Heating / Pump)
 * 
 * Control variables:
 * - enable       -> On/Off switch
 * - heating      -> Enables/disables heating
 * - waterPump    -> Enables/disables the water pump
 * - timeIsUp     -> Cooking is done
 * 
 * Globals:
 * - targetTemp   -> The target cooking temperature
 * - currentTemp  -> The current water temperature
 * - startTime    -> Time that cooking started (milliseconds)
 * - offsetTime   -> Total cooking time (milliseconds)
 * - endingTime   -> Ending time (milliseconds)
 * - timeIsUp     -> Compares if currentTime is greater than the endingTime
 * - enable       -> Gets the Blynk switch state
 * 
 * Policy for the actions:
 * - heating:     If (targetTemp > currentTemp) and (NOT timeIsUp)
 * - waterPump:   If (enable) and (NOT timeIsUp)
 */

/*
 *  Includes
 */
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BlynkSimpleEsp32_BT.h>
// #include <BlynkSimpleEsp32_BLE.h>
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
bool          timeIsUp      = false;
bool          enable        = false;
bool          prevEnable    = false;

/*
 *  Control variables
 */
bool          heating       = false;
bool          waterPump     = false;

/*
 *  Tasks
 */
TaskHandle_t TaskUpdateLocal;
TaskHandle_t TaskUpdateBlynk;
TaskHandle_t TaskActions;

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
  long cookingTime;

  if (t.hasStartTime()) {
    cookingTime = 
      t.getStartHour() * 3600 + 
      t.getStartMinute() * 60 +
      t.getStartSecond();
    cookingTime *= 1000;
  }

  offsetTime = cookingTime;
  endingTime = startTime + cookingTime;
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
 *  Method for updating local data
 */
void UpdateLocal (void *pvParameters) {

  // Loop
  for (;;) {

    /*
    * Globals:
    * - targetTemp   -> The target cooking temperature
    * - currentTemp  -> The current water temperature
    * - startTime    -> Time that cooking started (milliseconds)
    * - offsetTime   -> Total cooking time (milliseconds)
    * - endingTime   -> Ending time (milliseconds)
    * - timeIsUp     -> Compares if currentTime is greater than the endingTime
    * - enable       -> Gets the Blynk switch state
    */
    // targetTemp is covered by event-oriented paradigm
    // enable is covered by event-oriented paradigm
    // offsetTime is covered by event-oriented paradigm
    // The event-oriented paradigm
    Blynk.run();

    // currentTemp
    getTempCelsius();

    // startTime
    if (enable && !prevEnable) {
      startTime = millis();
    }
    prevEnable = enable;

    // endingTime
    endingTime = startTime + offsetTime;

    // timeIsUp
    timeIsUp = (millis() - endingTime) > 0;

    // Task delay
    sleep (DELAY_UPDATE_LOCAL);
  }
}

/*
 *  Method for updating stuff on Blynk app
 */
void UpdateBlynk (void *pvParameters) {

  // Loop
  for (;;) {

    // Sets the Blynk LED
    

    /*
    * Blynk outputs:
    * - LCD (Virtual Pin V0)
    * - Heating LED (Virtual Pin V1)
    */
    if (heating) {
      Blynk.virtualWrite(1, 1023);
    }
    else {
      Blynk.virtualWrite(1, 0);
    }
    // updateLCD();

    // Task delay
    sleep (DELAY_UPDATE_BLYNK);
  }
}

/*
 *  Method for updating stuff on Blynk app
 */
void Actions (void *pvParameters) {

  // Loop
  for (;;) {

    /*
     * Policy for the actions:
     * - heating:     If (targetTemp > currentTemp) and (NOT timeIsUp)
     * - waterPump:   If (enable) and (NOT timeIsUp)
    */
    // Water pump
    if ((targetTemp > currentTemp) && (!timeIsUp)) {
      digitalWrite(WATER_HEATING, HIGH);
    }
    else {
      digitalWrite(WATER_HEATING, LOW);
    }

    // Heating
    if ((enable) && (!timeIsUp)) {
      digitalWrite(WATER_PUMP, HIGH);
    }
    else {
      digitalWrite(WATER_PUMP, LOW);
    }

    // Task delay
    sleep (DELAY_ACTIONS);
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
  Blynk.setDeviceName("SousVide");
  Blynk.begin(token);

  // Initialize pins
  pinMode(WATER_PUMP, OUTPUT);

  xTaskCreatePinnedToCore(
                    UpdateLocal,       /* Task function. */
                    "TaskUpdateLocal", /* Name of task. */
                    STACK_SIZE_FLAGS_TASKS,     /* Stack size of task */
                    NO_PARAMETERS,              /* Parameters of the task */
                    PRIORITY_1,       /* Priority of the task (the higher, the more important) */
                    &TaskUpdateLocal,  /* Task handle to keep track of created task */
                    CORE_ONE);                 /* Pin task to core 1 */
  sleep(500);

  xTaskCreatePinnedToCore(
                    UpdateBlynk,       /* Task function. */
                    "TaskUpdateBlynk", /* Name of task. */
                    STACK_SIZE_FLAGS_TASKS,     /* Stack size of task */
                    NO_PARAMETERS,              /* Parameters of the task */
                    PRIORITY_1,       /* Priority of the task (the higher, the more important) */
                    &TaskUpdateBlynk,  /* Task handle to keep track of created task */
                    CORE_ZERO);                 /* Pin task to core 1 */
  sleep(500);

  xTaskCreatePinnedToCore(
                    Actions,       /* Task function. */
                    "TaskActions", /* Name of task. */
                    STACK_SIZE_FLAGS_TASKS,     /* Stack size of task */
                    NO_PARAMETERS,              /* Parameters of the task */
                    PRIORITY_1,       /* Priority of the task (the higher, the more important) */
                    &TaskActions,  /* Task handle to keep track of created task */
                    CORE_ZERO);                 /* Pin task to core 1 */
  sleep(500);

}

/*
 *  Loop
 */
void loop() {

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