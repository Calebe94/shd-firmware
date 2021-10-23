/***************************
 * INCLUDES
****************************/
#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include "sim/sim7070g.h"
#include "sensor/flowsensor.h"
#include "settings/settings.h"
#include "settings/devices.h"
#include "settings/reading.h"
#include "rs485/rs485.h"
#include "protocol/protocol.h"
#include "protocol/message_process.h"
#include "cmd/commands_handler.h"
#include "web/webservice.h"
#include "esp_log.h"
#include "esp32-hal-log.h"

#include <Ticker.h>

#define TAG                     "MAIN"
#define uS_TO_S_FACTOR          1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP           60          /* Time ESP32 will go to sleep (in seconds) */

Ticker reading_autosave;

void setup()
{
    // Set console baud rate
    SerialMon.begin(115200);
    delay(10);

    if(!SPIFFS.begin(false))
    {
        ESP_LOGE(TAG, "SPIFFS Mount Failed");
        return;
    }

    settings_load();
    reading_load();
    flowsensor_set_litros((float)reading_get());
    flowsensor_init();
    rs485_init();

#ifdef CONTROLLER_FIRMWARE
    devices_load();
    settings_set_mode(CONTROLLER_DEVICE);
    settings_set_id(255);
    settings_update();
    sim7070g_init();
    protocol_init(CONTROLLER, 255);
    WiFi.softAP("hidrometro-controlador", "hidrometro");
#else
    settings_set_mode(PERIPHERAL_DEVICE);
    settings_update();
    char ssid[30];
    snprintf(ssid, 30, "hidrometro-periferico-%d", settings_get_id());
    WiFi.softAP(ssid, "hidrometro");
    protocol_init(PERIPHERAL, settings_get_id());
#endif

    webservice_init();
    // Save reading every 5 minutes
    reading_autosave.attach_ms(5*60*1000, []() {
        reading_set((float)flowsensor_get_litros());
        reading_update();
    });
#if defined(CONTROLLER_FIRMWARE) && !defined(DEBUG)
    xTaskCreate(get_readings_timer_callback, "get_readings_timer_callback", 8192, NULL, 5, NULL);
    commands_handler_init();
    sim7070g_resume_event_handler();
#endif
}

void loop()
{
#ifdef DEBUG
    while (SerialAT.available())
    {
        Serial.write(SerialAT.read());
    }

    while (Serial.available())
    {
        SerialAT.write(Serial.read());
    }
#endif
}

