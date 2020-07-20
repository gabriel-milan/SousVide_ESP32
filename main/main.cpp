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
#include "Arduino.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "include/SousVide.h"
#include "OneWire.h"
#include "DallasTemperature.h"

/*
 *  DS18B20 declaration
 */
OneWire oneWire (ONE_WIRE_BUS);
DallasTemperature sensors (&oneWire);

extern "C" void app_main(void)
{
}