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
        if (cJSON_IsNumber(json_id))
        {
            global_settings.id = json_id->valueint;
        }

        if (cJSON_IsString(json_mode))
        {
            if(strcmp("master", json_mode->valuestring) == 0)
            {
                global_settings.mode = MASTER_DEVICE;
            }
            else
            {
                global_settings.mode = SLAVE_DEVICE;
            }
        }
    }
}

void settings_update()
{
    char *string = NULL;
    FILE *json_file = NULL;
    cJSON *json_settings = NULL;
    cJSON *json_id = NULL;
    cJSON *json_mode = NULL;

    json_settings = cJSON_CreateObject();
    json_id = cJSON_CreateNumber(global_settings.id);
    json_mode = cJSON_CreateString(((uint8_t)global_settings.mode==1?"master":"slave"));

    cJSON_AddItemToObject(json_settings, "id", json_id);
    cJSON_AddItemToObject(json_settings, "mode", json_mode);
    string = cJSON_Print(json_settings);
    ESP_LOGI(TAG, "JSON File: \n%s", string);
    json_file = fopen(SETTINGS_FILE, "w");
    fprintf(json_file, string);
    fclose(json_file);
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
