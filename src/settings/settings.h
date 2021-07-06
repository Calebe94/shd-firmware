#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <stdint.h>

typedef enum {
    SLAVE=0,
    MASTER=1
} settings_peer_t;

typedef struct settings {
    uint8_t id;
    settings_peer_t peer;
} settings_t;

void settings_load(settings_t *);

void settings_update(settings_t);

uint8_t settings_get_id(void);

void settings_set_id(uint8_t);

settings_peer_t settings_get_peer(void);

void settings_set_peer(settings_peer_t);

#endif
