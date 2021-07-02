#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <stdbool.h>
#include <stdint.h>

#define MAX_DATA_LENGTH 128

typedef enum {
    GET=0,
    SET=1
} protocol_action_t;

typedef enum {
    SLAVE=0,
    MASTER
} protocol_peer_t;

typedef struct protocol_data
{
    uint8_t id;
    uint8_t action;
    uint8_t length;
    uint8_t data[MAX_DATA_LENGTH];
} protocol_data_raw_t;

typedef struct protocol_settings
{
    uint8_t id;
    protocol_peer_t peer;
} protocol_settings_t;

bool protocol_init(protocol_peer_t, uint8_t);

bool protocol_message_parse(char *, protocol_data_raw_t *);

bool protocol_create_message(protocol_data_raw_t, char *);

bool protocol_check_id(protocol_data_raw_t);

#endif
