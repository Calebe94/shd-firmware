#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "cJSON_Utils.h"

#include "settings.h"

static settings_t global_settings;

void settings_load(void)
{
    FILE *f = fopen("/spiffs/settings.json", "rb");
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

void settings_update(settings_t new_settings)
{
    global_settings = new_settings;
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
