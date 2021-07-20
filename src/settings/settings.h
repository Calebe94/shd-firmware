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
    char phone[20]; // O máximo pode ser 18 com terminador nulo. 20 é o coeficiente de cagaço.
} settings_t;

void settings_load(void);

void settings_update();

uint8_t settings_get_id(void);

void settings_set_id(uint8_t);

void settings_set_phone(char*);

char *settings_get_phone(void);

settings_mode_t settings_get_mode(void);

void settings_set_mode(settings_mode_t);

#endif
