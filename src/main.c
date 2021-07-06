/***************************
 * INCLUDES
****************************/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "lwip/apps/netbiosns.h"

#include "tiny_wifi.h"
#include "tiny_webservice.h"
#include "flash_storage.h"

#include "serial/console.h"
#include "sensor/flowsensor.h"
#include "rs485/rs485.h"
#include "protocol/protocol.h"
#include "protocol/message_process.h"

static const char * TAG = "MAIN";

/***************************
 * MAIN
****************************/
void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());

    console_init();
    flowsensor_init();
    rs485_init();

    protocol_init(SLAVE, 1);
    xTaskCreate(message_process_handler, "message_process_handler", 4096, NULL, 12, NULL);
    wifi_ap_init(CONFIG_WIFI_AP_SSID, CONFIG_WIFI_AP_PASS);
    #if CONFIG_USE_MDNS
    init_mdns();
    netbiosns_init();
    #endif
    ESP_ERROR_CHECK(init_flash_storage("/spiffs", "spiffs"));
    
    // Use POSIX and C standard library functions to work with files.
    // First create a file.
    ESP_LOGI(TAG, "Opening file");
    FILE* f = fopen("/spiffs/hello.txt", "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fprintf(f, "Hello World!\n");
    fclose(f);
    ESP_LOGI(TAG, "File written");

    // Check if destination file exists before renaming
    struct stat st;
    if (stat("/spiffs/foo.txt", &st) == 0) {
        // Delete it if it exists
        unlink("/spiffs/foo.txt");
    }

    // Rename original file
    ESP_LOGI(TAG, "Renaming file");
    if (rename("/spiffs/hello.txt", "/spiffs/foo.txt") != 0) {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }

    // Open renamed file for reading
    ESP_LOGI(TAG, "Reading file");
    f = fopen("/spiffs/foo.txt", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);
    // strip newline
    char* pos = strchr(line, '\n');
    if (pos)
    {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);
    
    ESP_LOGI(TAG, "Reading file");
    f = fopen("/spiffs/settings.json", "rb");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  

    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    string[fsize] = 0;
    
    ESP_LOGI(TAG, "size: %d - content: %s", (int)fsize,string);

    while(1)
    {
        /*
        uint8_t data[128];
        int len = rs485_read(data);
        protocol_data_raw_t data_parsed;
        if(protocol_message_parse((char*)data, &data_parsed))
        {
            printf("A mensagem %s é para mim! \n", (char *)data);
        }

        data[len] = '\0';
        if(len > 0)
        {
            printf("Recebido: %s\n", (char*)data);
        }
        printf("pulses: %d - litros: %f\n", flowsensor_get_pulses(), flowsensor_get_litros());
        */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
