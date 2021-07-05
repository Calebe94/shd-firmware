#ifndef _FLASH_STORAGE_
#define _FLASH_STORAGE_

#include "esp_err.h"

esp_err_t init_flash_storage(char * base_path, char * partition_label);

#endif