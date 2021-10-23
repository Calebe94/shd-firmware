#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "esp_log.h"
#include <ArduinoJson.h>

#include "devices.h"

static const char *TAG = "DEVICES";

static device_id_t devices_ids[MAX_DEV_ID_LENGTH];
static uint8_t devices_ids_length;

void devices_load(void)
{
    ESP_LOGD(TAG, "Iniciando a lista de dispositivos...");
    FILE *f = fopen(DEVICES_FILE, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = (char*)malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    string[fsize] = 0;

    ESP_LOGD(TAG, "%s", string);

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, string);

    JsonArray devices_array = doc["devices"].as<JsonArray>();
    
    for(JsonVariant device: devices_array)
    {
        device_add(device.as<int>());
        ESP_LOGD(TAG, "device: %d", device.as<int>());
    }
}

void devices_update(void)
{
    ESP_LOGD(TAG, "Gravando os dispositivos...");
    FILE *json_file = NULL;
    String devices_string = "";
    DynamicJsonDocument doc(1024);

    for (uint8_t index = 0; index < devices_ids_length; index++)
    {
        doc["devices"][index] = device_get_id(index);
    }

    serializeJsonPretty(doc, devices_string);
    const char *devices_char = devices_string.c_str();
    ESP_LOGD(TAG, "%s", devices_char);
    
    json_file = fopen(DEVICES_FILE, "w");
    fprintf(json_file, devices_char);
    fclose(json_file);
    ESP_LOGD(TAG, "Dispositivos gravados!");
}

bool device_add(int device_id)
{
    bool status = false;
    
    if(devices_ids_length < MAX_DEV_ID_LENGTH && device_check_duplication(device_id) == false)
    {
        devices_ids[devices_ids_length++] = device_id;
        status = true;
    }
    return status;
}

int device_index_of(int device_id)
{
    int index_of = 0;

    for (int index = 0; index < devices_ids_length; index++)
    {
        if(devices_ids[index] == device_id)
        {
            index_of = index;
            break;
        }
    }

    return index_of;
}

bool device_delete(int device_id)
{
    bool status = false;
    
    if (device_id < MAX_DEV_ID_LENGTH && device_id > 0)
    {
        for (int index = device_index_of(device_id); index < devices_ids_length - 1; index++)
        {
            devices_ids[index] = devices_ids[index+1];
        }
        devices_ids_length--;
        status = true;
    }

    return status;
}

device_id_t device_get_id(int index)
{
    device_id_t id = 0;
    if (index < MAX_DEV_ID_LENGTH)
    {
        id = devices_ids[index];
    }

    return id;
}

int devices_get_length(void)
{
    return devices_ids_length;
}

bool device_check_duplication(int device_id)
{
    bool status = false;

    for (uint8_t index = 0; index < devices_ids_length; index++)
    {
        if(devices_ids[index] == device_id)
        {
            status = true;
            break;
        }
    }

    return status;
}

