#ifdef __cplusplus
    extern "C" {
#endif
#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <stdint.h>
#include <stdbool.h>

#define SETTINGS_FILE "/spiffs/settings.json"
#define MAX_PHONES      12

typedef enum {
    PERIPHERAL_DEVICE=0,
    CONTROLLER_DEVICE=1
} settings_mode_t;

typedef struct settings {
    uint8_t id;
    settings_mode_t mode;
    char phone[20][20]; // O máximo pode ser 18 com terminador nulo. 20 é o coeficiente de cagaço.
    char local[128];
    int interval;
} settings_t;

void settings_load(void);

void settings_update(void *argv);

uint8_t settings_get_id(void);

void settings_set_id(uint8_t);

bool settings_set_phone(uint8_t, char*);

char *settings_get_phone(uint8_t);

uint8_t settings_find_phone_id(char *);

bool settings_delete_phone_by_id(uint8_t);

uint8_t settings_get_phones_list_length();

settings_mode_t settings_get_mode(void);

void settings_set_mode(settings_mode_t);

void settings_set_local(char*);

char *settings_get_local(void);

int settings_get_interval(void);

void settings_set_interval(int);

#endif
#ifdef __cplusplus
}
#endif
