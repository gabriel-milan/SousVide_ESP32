/*
 *  Includes
 */
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>
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
BlynkTimer timerData;

/*
 *  Blynk LCD
 */
WidgetLCD lcd(V0);

/*
 *  Globals
 */
unsigned int  targetTemp    = 0;
unsigned int  currentTemp   = 0;
unsigned int  heating       = 0;
long          startTime     = 0;
long          offsetTime    = 0;
long          endingTime    = 0;
bool          enable        = false;

/*
 *  Method that sleeps using the vTaskDelay function
 */
void sleep (int ms) {
  vTaskDelay(pdMS_TO_TICKS(ms));
}

/*
 *  Script that makes the initialization
 */
void start () {
  startTime = millis();
  if (offsetTime != 0) {
    endingTime = startTime + offsetTime;
  }
}

/*
 *  Script that stops the sous vide
 */
void stop () {
  // TODO
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
void updateHeating () {
  if (millis() >= endingTime) {
    heating = 0;
  }
  else {
    if (currentTemp < targetTemp) {
      heating = 1023;
    }
    else {
      heating = 0;
    }
  }
}

/*
 *  Helper method that prints temperature into the LCD
 */
void printTemperature (unsigned int temp) {
  lcd.print (0, 7, String(temp));
}

/*
 *  Helper method that prints the timer into the LCD
 */
void printTimer (long start) {
  long refTime;
  if (start <= 0) {
    refTime = 0;
  }
  else {
    refTime = millis() - start;
  }
  float div_seconds = refTime / 1000.f;
  int seconds = (int) div_seconds;
  float div_minutes = seconds / 60.f;
  int minutes = (int) div_minutes;
  float div_hours = minutes / 60.f;
  int hours = (int) div_hours;
  lcd.print (1, 6, String(hours));
  lcd.print (1, 9, String(minutes));
  lcd.print (1, 12, String(seconds));
}

/*
 *  Methods for Blynk
 */
// V0 - LCD that shows current temperature and cooking time
void sendLCD () {
  printTemperature(currentTemp);
  printTimer(startTime);
}
// V1 - LED that shows if heating is enabled
void sendHeating () {
  Blynk.virtualWrite(1, heating);
}
// Sending everything
void sendEverything () {
  sendLCD();
  sendHeating();
}
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
  bool prevEnable = enable;
  enable = param.asInt();
  if (enable && !prevEnable) {
    start();
  }
  else if (!enable) {
    stop();
  }
}

/*
 *  Method that updates internal things
 */
void updateData () {
  getTempCelsius();
  updateHeating();
}

/*
 *  Setup
 */
void setup() {

  // Initializing serial connection
  Serial.begin(SERIAL_BAUD_RATE);

  // Initializing the sensor
  sensors.begin();

  // Clear the LCD and print headers
  lcd.clear();
  lcd.print (0, 0, "Temp:  --.- ºC");
  lcd.print (0, 1, "Time: --:--:--");

  // Initialize Blynk
  Blynk.setDeviceName("SousVide");
  Blynk.begin(token);
  timerSend.setInterval(SEND_INTERVAL, sendEverything);
  timerData.setInterval(GETS_INTERVAL, updateData);

}

/*
 *  Loop
 */
void loop() {

  timerSend.run();
  timerData.run();
  Blynk.run();

  // Serial.print("Current temp: ");
  // Serial.print(currentTemp);
  // Serial.print("ºC / Target temp: ");
  // Serial.print(targetTemp);
  // Serial.print("ºC / Heating: ");
  // Serial.println(heating);

}