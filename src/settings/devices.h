#ifndef _DEVICES_H_
#define _DEVICES_H_

#include <stdint.h>
#include <stdbool.h>

#define DEVICES_FILE "/spiffs/devices.json"
#define MAX_DEV_ID_LENGTH 255 // It goes from 1 to 254

typedef uint8_t device_id_t;

void devices_load(void);

void devices_update(void);

bool device_add(int);

bool device_delete(int);

device_id_t device_get_id(int);

int devices_get_length(void);

bool device_check_duplication(int);

#endif
