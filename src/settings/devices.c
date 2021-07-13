#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "esp_log.h"
#include "cJSON_Utils.h"

#include "devices.h"

static const char *TAG = "DEVICES";

static device_id_t devices_ids[MAX_DEV_ID_LENGTH];
static uint8_t devices_ids_length;

void devices_load(void)
{
    ESP_LOGI(TAG, "Iniciando a lista de dispositivos...");
    FILE *f = fopen(DEVICES_FILE, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    string[fsize] = 0;

    cJSON * json = cJSON_Parse(string);
    cJSON *device_id_json = NULL;
    cJSON *devices_ids_json = NULL;

    ESP_LOGI(TAG, "%s", string);
    devices_ids_json = cJSON_GetObjectItemCaseSensitive(json, "devices");

    cJSON_ArrayForEach(device_id_json, devices_ids_json)
    {
        if(cJSON_IsNumber(device_id_json) )
        {
            device_add(device_id_json->valueint);
        }
    }

    ESP_LOGI(TAG, "Resgatando ids da lista...");
    for(uint8_t index = 0; index < devices_ids_length; index++)
    {
        ESP_LOGI(TAG, "Device List: %d", devices_ids[index]);
    }
}

void devices_update(void)
{
    char *string = NULL;
    FILE *json_file = NULL;

    cJSON *json_devices = NULL;
    cJSON *json_id = NULL;
    cJSON *json_devices_array = NULL;

    json_devices = cJSON_CreateObject();
    json_devices_array = cJSON_CreateArray();

    cJSON_AddItemToObject(json_devices, "devices", json_devices_array);

    for (uint8_t index = 0; index < devices_ids_length; index++)
    {
        json_id = cJSON_CreateNumber(devices_ids[index]);
        cJSON_AddItemToArray(json_devices_array, json_id);
    }

    string = cJSON_Print(json_devices);
    ESP_LOGI(TAG, "JSON File: \n%s", string);
    
    json_file = fopen(DEVICES_FILE, "w");
    fprintf(json_file, string);
    fclose(json_file);
    cJSON_Delete(json_devices);
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

bool device_delete(int device_id)
{
    return true;
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
