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
#include "esp_log.h"
#include "esp32-hal-log.h"

#define TAG                     "MAIN"
#define uS_TO_S_FACTOR          1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP           60          /* Time ESP32 will go to sleep (in seconds) */

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
    flowsensor_init();
#ifdef CONTROLLER_FIRMWARE
    devices_load();
    sim7070g_init();
#endif

    ESP_LOGI(TAG, "Wait...");

    int retry = 5;
    bool reply= false;
    while (!(reply = sim7070g_turn_on()) && retry--);

#ifdef DEBUG
    if (reply)
    {
        ESP_LOGI(TAG, F("***********************************************************"));
        ESP_LOGI(TAG, F(" You can now send AT commands"));
        ESP_LOGI(TAG, F(" Enter \"AT\" (without quotes), and you should see \"OK\""));
        ESP_LOGI(TAG, F(" If it doesn't work, select \"Both NL & CR\" in Serial Monitor"));
        ESP_LOGI(TAG, F(" DISCLAIMER: Entering AT commands without knowing what they do"));
        ESP_LOGI(TAG, F(" can have undesired consiquinces..."));
        ESP_LOGI(TAG, F("***********************************************************\n"));
    }
    else
    {
        ESP_LOGI(TAG, F("***********************************************************"));
        ESP_LOGI(TAG, F(" Failed to connect to the modem! Check the baud and try again."));
        ESP_LOGI(TAG, F("***********************************************************\n"));
    }
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


