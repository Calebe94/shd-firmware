#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <stdbool.h>

typedef enum {
    GET=0,
    SET=1
} protocol_action_t;

void protocol_init(void);

bool protocol_message_parse(char *);

#endif
