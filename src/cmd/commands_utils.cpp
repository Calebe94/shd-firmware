#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "esp_log.h"
#include "esp32-hal-log.h"

bool command_validate_phone_number(char *phone)
{
    bool status = false;
    int digits = 0;
    char number[15];

    ESP_LOGD(__func__, "Verificando se %s é um telefone válido...", phone);
    sscanf(phone, " %11[0-9] %n", number, &digits);

    if (digits == 0 && number[digits])
    {
        ESP_LOGD(__func__, "Sem dígitos ou dígitos extras.");
        status = false;
    }
    else
    {
        if (strlen(number) == 11 || strlen(number) == 9)
        {
            ESP_LOGD(__func__, "é um telefone válido!");
            status = true;
        }
        else
        {
            ESP_LOGD(__func__, "não é um telefone válido!");
            status = false;
        }
    }
    
    return status;
}
