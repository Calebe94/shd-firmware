#ifndef _SIM7070G_UTILS_H_
#define _SIM7070G_UTILS_H_
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "Arduino.h"

int sim7070g_from_cmti_get_id(const char *);

String sim7070g_from_cmgr_get_phone(const char *);

String sim7070g_from_cmgr_get_message(const char *);

#endif
