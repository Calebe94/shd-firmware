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
        cJSON * json_peer = cJSON_GetObjectItemCaseSensitive(json, "peer");
        if (cJSON_IsNumber(json_id))
        {
            global_settings.id = json_id->valueint;
        }

        if (cJSON_IsString(json_peer))
        {
            if(strcmp("master", json_peer->valuestring) == 0)
            {
                global_settings.peer = MASTER_DEVICE;
            }
            else
            {
                global_settings.peer = SLAVE_DEVICE;
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
    cJSON *json_peer = NULL;

    json_settings = cJSON_CreateObject();
    json_id = cJSON_CreateNumber(global_settings.id);
    json_peer = cJSON_CreateString(((uint8_t)global_settings.peer==1?"master":"slave"));

    cJSON_AddItemToObject(json_settings, "id", json_id);
    cJSON_AddItemToObject(json_settings, "peer", json_peer);
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

settings_peer_t settings_get_peer(void)
{
    return global_settings.peer;
}

void settings_set_peer(settings_peer_t peer)
{
    global_settings.peer = peer;
}
