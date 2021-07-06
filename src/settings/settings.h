#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <stdint.h>

#define SETTINGS_FILE "/spiffs/settings.json"

typedef enum {
    SLAVE_DEVICE=0,
    MASTER_DEVICE=1
} settings_peer_t;

typedef struct settings {
    uint8_t id;
    settings_peer_t peer;
} settings_t;

void settings_load(void);

void settings_update();

uint8_t settings_get_id(void);

void settings_set_id(uint8_t);

settings_peer_t settings_get_peer(void);

void settings_set_peer(settings_peer_t);

#endif
