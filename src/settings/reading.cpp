#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "esp_log.h"
#include <ArduinoJson.h>
#include "reading.h"

static const char *TAG = "LEITURAS";
static float reading;


/*
 * Funções de controle e manuseio das informações de leitura do dispositivo(controlador e perfiférico).
 *
 * */
void reading_load()
{
    ESP_LOGD(TAG, "Resgatando a última leitura na memória...");
    FILE *f = fopen(READING_FILE, "rb");
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

   reading_set((float)doc["leitura"]);
}

void reading_update()
{
    FILE *json_file = NULL;
    String settings_string = "";
    DynamicJsonDocument doc(1024);
    doc["leitura"] = (float)reading_get();

    serializeJsonPretty(doc, settings_string);
    const char *settings_char = settings_string.c_str();
    ESP_LOGD(TAG, "%s", settings_char);
    json_file = fopen(READING_FILE, "w");
    fprintf(json_file, settings_char);
    fclose(json_file);
}

float reading_get()
{
    return reading;
}

void reading_set(float aux)
{
    reading = aux;
}

