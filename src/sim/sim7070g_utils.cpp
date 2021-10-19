#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "Arduino.h"
#include "sim7070g_utils.h"

int sim7070g_from_cmti_get_id(const char *cmti_response)
{
    int id = -1;
    sscanf(cmti_response, "+CMTI: \"SM\",%d", &id);
    return id;
}

String sim7070g_from_cmgr_get_phone(const char *cmgr_response)
{
    String phone = "";

    char cmgr_phone[30], cmgr_message_status[30], cmgr_sts[30], cmgr_date[30], cmgr_hour[30];
    if(sscanf(cmgr_response, "+CMGR: %[^','],%[^','],%[^','],%[^','],%[^',']", cmgr_message_status, cmgr_phone, cmgr_sts, cmgr_date, cmgr_hour))
    {
        phone = String(cmgr_phone);
        phone.replace("\"", " ");
        phone.trim();
    }

    return phone;
}

String sim7070g_from_cmgr_get_message(const char *cmgr_response)
{
    String phone = "";
    char *token;
    char aux_cmgr_response[512];

    strncpy(aux_cmgr_response, cmgr_response, 512);
    token = strtok(aux_cmgr_response, "\n");

    while( token != NULL )
    {
        token = strtok(NULL, "\n");
        phone += String(token);
    }

    return phone;
}
