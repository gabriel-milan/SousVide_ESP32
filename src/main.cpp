/*
 *  Includes
 */
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "credentials.h"
#include "macros.h"

/*
 *  DS18B20 declaration
 */
OneWire oneWire (ONE_WIRE_BUS);
DallasTemperature sensors (&oneWire);

/*
 *  Timers
 */
BlynkTimer timerSend;
BlynkTimer timerGets;

/*
 *  Globals
 */
unsigned int targetTemp   = -1;
unsigned int currentTemp  = -1;
unsigned int addWater     =  0;

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
 * Method that updates LED
 */
void cmpTarget () {
  if (currentTemp < targetTemp) {
    addWater = 1023;
  }
  else {
    addWater = 0;
  }
}

/*
 *  Send methods for Blynk
 */
void sendCurrentTemp () {
  Blynk.virtualWrite(0, currentTemp);
}
void sendAddWater () {
  Blynk.virtualWrite(2, addWater);
}
void sendEverything () {
  sendCurrentTemp();
  sendAddWater();
}
void getEverything () {
  getTempCelsius();
  cmpTarget();
}

/*
 *  Blynk stuff
 */
// Writing the current temperature into V0
BLYNK_READ(V0) {
  sendCurrentTemp();
}
// Reading the target temperature from V1
BLYNK_WRITE(V1) {
  targetTemp = param.asInt();
}
BLYNK_READ(V2) {
  sendAddWater();
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
  Blynk.begin(token, ssid, password);
  timerSend.setInterval(SEND_INTERVAL, sendEverything);
  timerGets.setInterval(GETS_INTERVAL, getEverything);

}

/*
 *  Loop
 */
void loop() {

  timerSend.run();
  timerGets.run();
  Blynk.run();

  Serial.print("Current temp: ");
  Serial.print(currentTemp);
  Serial.print("ºC / Target temp: ");
  Serial.print(targetTemp);
  Serial.print("ºC / AddWater: ");
  Serial.println(addWater);

}