#ifndef _DEVICES_H_
#define _DEVICES_H_

#include <stdint.h>
#include <stdbool.h>

#define DEVICES_FILE "/spiffs/devices.json"
#define MAX_DEV_ID_LENGTH 255 // It goes from 1 to 254

typedef uint8_t device_id_t;

void devices_init(void);

void devices_load(void);

void devices_update(void);

bool devices_add(int);

device_id_t devices_get_id(int);

int devices_get_length(void);

bool devices_check_duplication(int);

#endif
