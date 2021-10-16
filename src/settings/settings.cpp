#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include <ArduinoJson.h>

#include "settings.h"

static const char * TAG = "SETTINGS";

static settings_t global_settings;

void settings_load(void)
{
    ESP_LOGD(TAG, "Iniciando as configurações...");
    FILE *f = fopen(SETTINGS_FILE, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = (char*)malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    string[fsize] = 0;
    ESP_LOGI(TAG, "%s", string);
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, string);
    global_settings.id = (int)doc["id"];
    if(strcmp("controller", (const char*)doc["mode"]) == 0)
    {
        global_settings.mode = CONTROLLER_DEVICE;
    }
    else
    {
        global_settings.mode = PERIPHERAL_DEVICE;
    }

    JsonArray array = doc["phones"].as<JsonArray>();
    int index = 0;
    for(JsonVariant phone : array)
    {
        String phone_string = phone.as<String>();
        const char *phone_char = phone_string.c_str();
        strncpy(global_settings.phone[index++], phone_char, 20);
    }
    strncpy(global_settings.local, doc["local"], 128);
    global_settings.interval = (int)doc["interval"];

    ESP_LOGD(TAG, "Configurações carregadas!");
    ESP_LOGD(TAG, "SETTINGS.id: %d", global_settings.id);
    ESP_LOGD(TAG, "SETTINGS.mode: %d", global_settings.mode);
    for(uint8_t index = 0; index < settings_get_phones_list_length(); index++)
    {
        ESP_LOGD(TAG, "SETTINGS.phones[%d]: %s", index, global_settings.phone[index]);
    }
    ESP_LOGD(TAG, "SETTINGS.local: %s", global_settings.local);
    ESP_LOGD(TAG, "SETTINGS.interval: %d", global_settings.interval);
}

void settings_update()
{
    ESP_LOGD(TAG, "Gravando as configurações...");
    FILE *json_file = NULL;
    String settings_string = "";
    DynamicJsonDocument doc(1024);
    doc["id"] = (int)global_settings.id;
    doc["mode"] = ((int)global_settings.mode==1)?"controller":"peripheral";
    doc["local"] = global_settings.local;
    doc["interval"] = global_settings.interval;
    for(uint8_t index = 0; index < settings_get_phones_list_length(); index++)
    {
        doc["phones"][index] = global_settings.phone[index];
    }
    serializeJsonPretty(doc, settings_string);
    const char *settings_char = settings_string.c_str();
    ESP_LOGD(TAG, "%s", settings_char);
    json_file = fopen(SETTINGS_FILE, "w");
    fprintf(json_file, settings_char);
    fclose(json_file);
    ESP_LOGD(TAG, "Configurações gravadas!");
}

uint8_t settings_get_id(void)
{
    return global_settings.id;
}

void settings_set_id(uint8_t id)
{
    global_settings.id = id;
}

settings_mode_t settings_get_mode(void)
{
    return global_settings.mode;
}

void settings_set_mode(settings_mode_t mode)
{
    global_settings.mode = mode;
}

bool settings_set_phone(uint8_t index, char *phone)
{
    bool status = false;
    if( index < 20 )
    {
        strncpy(global_settings.phone[index], phone, 20);
        status = true;
    }
    return status;
}

char *settings_get_phone(uint8_t index)
{
    char *phone = NULL;
    if(index < 20)
    {
        phone = global_settings.phone[index];
    }
    return phone;
}

uint8_t settings_find_phone_id(char *phone)
{
    uint8_t id = 255;
    for(uint8_t index = 0; index < settings_get_phones_list_length(); index++)
    {
        if(strcmp(global_settings.phone[index], phone) ==0)
        {
            id = index;
            break;
        }
    }
    return id;
}

bool settings_delete_phone_by_id(uint8_t phone_index)
{
    bool status = false;

    if (phone_index < MAX_PHONES)
    {
        memset(global_settings.phone[phone_index], 0, 20);
        for (uint8_t index = phone_index; index < settings_get_phones_list_length() - 1; index++)
        {
            memcpy(global_settings.phone[index], global_settings.phone[index+1], 20);
        }
        status = true;
    }

    return status;
}

uint8_t settings_get_phones_list_length()
{
    uint8_t lenght = 0;
    for (uint8_t index = 0; index < MAX_PHONES; index++)
    {
        if(strcmp(global_settings.phone[index], "") == 0)
        {
            lenght = index;
            break;
        }
    }
    return lenght;
}

void settings_set_local(char *local)
{
    strncpy(global_settings.local, local, 128);
}

char *settings_get_local(void)
{
    return (char*)global_settings.local;
}

int settings_get_interval(void)
{
    return (int)global_settings.interval;
}

void settings_set_interval(int interval)
{
    global_settings.interval = interval;
}

