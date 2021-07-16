#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <stdint.h>

#define SETTINGS_FILE "/spiffs/settings.json"

typedef enum {
    PERIPHERAL_DEVICE=0,
    CONTROLLER_DEVICE=1
} settings_mode_t;

typedef struct settings {
    uint8_t id;
    settings_mode_t mode;
} settings_t;

void settings_load(void);

void settings_update();

uint8_t settings_get_id(void);

void settings_set_id(uint8_t);

settings_mode_t settings_get_mode(void);

void settings_set_mode(settings_mode_t);

#endif
