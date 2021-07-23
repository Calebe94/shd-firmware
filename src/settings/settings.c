#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "esp_log.h"
#include "cJSON_Utils.h"

#include "settings.h"

static const char * TAG = "SETTINGS";

static settings_t global_settings;

void settings_load(void)
{
    FILE *f = fopen(SETTINGS_FILE, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    string[fsize] = 0;

    cJSON * json = cJSON_Parse(string);
    if(json != NULL)
    {
        cJSON * json_id = cJSON_GetObjectItemCaseSensitive(json, "id");
        cJSON * json_mode = cJSON_GetObjectItemCaseSensitive(json, "mode");
        cJSON * json_phone = cJSON_GetObjectItemCaseSensitive(json, "phone");
        cJSON * json_local = cJSON_GetObjectItemCaseSensitive(json, "local");
        cJSON * json_interval = cJSON_GetObjectItemCaseSensitive(json, "interval");

        if (cJSON_IsNumber(json_id))
        {
            global_settings.id = json_id->valueint;
        }

        if (cJSON_IsString(json_mode))
        {
            if(strcmp("controller", json_mode->valuestring) == 0)
            {
                global_settings.mode = CONTROLLER_DEVICE;
            }
            else
            {
                global_settings.mode = PERIPHERAL_DEVICE;
            }
        }

        if (cJSON_IsString(json_phone))
        {
            strncpy(global_settings.phone, json_phone->valuestring, 20);
        }

        if (cJSON_IsString(json_local))
        {
            strncpy(global_settings.local, json_local->valuestring, 128);
        }

        if (cJSON_IsNumber(json_interval))
        {
            global_settings.interval = json_interval->valueint;
        }
    }
    cJSON_Delete(json);
}

void settings_update()
{
    char *string = NULL;
    FILE *json_file = NULL;
    cJSON *json_settings = NULL;
    cJSON *json_id = NULL;
    cJSON *json_mode = NULL;
    cJSON *json_phone = NULL;
    cJSON *json_local = NULL;
    cJSON *json_interval = NULL;

    json_settings = cJSON_CreateObject();
    json_id = cJSON_CreateNumber(global_settings.id);
    json_mode = cJSON_CreateString(((uint8_t)global_settings.mode==1?"controller":"peripheral"));
    json_phone = cJSON_CreateString(global_settings.phone);
    json_local = cJSON_CreateString(global_settings.local);
    json_interval = cJSON_CreateNumber(global_settings.interval);

    cJSON_AddItemToObject(json_settings, "id", json_id);
    cJSON_AddItemToObject(json_settings, "mode", json_mode);
    cJSON_AddItemToObject(json_settings, "phone", json_phone);
    cJSON_AddItemToObject(json_settings, "local", json_local);
    cJSON_AddItemToObject(json_settings, "interval", json_interval);

    string = cJSON_Print(json_settings);

    ESP_LOGI(TAG, "JSON File: \n%s", string);
    json_file = fopen(SETTINGS_FILE, "w");
    fprintf(json_file, string);
    fclose(json_file);
    cJSON_Delete(json_settings);
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

void settings_set_phone(char *phone)
{
    strncpy(global_settings.phone, phone, 20);
}

char *settings_get_phone(void)
{
    return global_settings.phone;
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
