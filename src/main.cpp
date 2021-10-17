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
#include "rs485/rs485.h"
#include "protocol/protocol.h"
#include "protocol/message_process.h"
#include "web/webservice.h"
#include "esp_log.h"
#include "esp32-hal-log.h"

#define TAG                     "MAIN"
#define uS_TO_S_FACTOR          1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP           60          /* Time ESP32 will go to sleep (in seconds) */

void setup()
{
    int retry = 5;
    bool reply= false;
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
    rs485_init();

#ifdef CONTROLLER_FIRMWARE
    devices_load();
    sim7070g_init();
    protocol_init(CONTROLLER, 255);
    WiFi.softAP("hidrometro-controlador", "hidrometro");
#else
    char ssid[30];
    snprintf(ssid, 30, "hidrometro-periferico-%d", settings_get_id());
    WiFi.softAP(ssid, "hidrometro");
    protocol_init(PERIPHERAL, settings_get_id());
#endif

    webservice_init();
    ESP_LOGI(TAG, "Wait...");

    while (!(reply = sim7070g_turn_on()) && retry--);
    if(reply)
    {
        if(sim7070g_send_sms("41998271302", "Este é um teste, com Arduino!"))
        {
            ESP_LOGI(TAG, "Mensagem enviada com sucesso!");
        }
    }
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
#ifdef CONTROLLER_FIRMWARE
    xTaskCreate(get_readings_timer_callback, "get_readings_timer_callback", 8192, NULL, 10, NULL);
#endif
}

void loop()
{
    ESP_LOGI(TAG, "litros: %f", flowsensor_get_litros());
    delay(1000);
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


