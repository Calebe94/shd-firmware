#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "protocol.h"

protocol_settings_t device_settings;

bool protocol_init(protocol_mode_t mode, uint8_t id)
{
    bool status = false;
    if (mode == CONTROLLER)
    {
        device_settings.mode = CONTROLLER;
        device_settings.id = 255;
    }
    else
    {
        device_settings.mode = PERIPHERAL;
        /* ID = 0 is BROADCAST*/
        if (id != 0)
        {
            device_settings.id = id;
            status = true;
        }
    }
    return status;
}

bool protocol_message_parse(char *serial_data, protocol_data_raw_t *data)
{
    bool status = false;

    if ( serial_data != NULL )
    {
        data->id = (uint8_t)serial_data[0];
        data->action = (uint8_t)serial_data[1];
        data->length = (uint8_t)serial_data[2];
        for (uint8_t index = 0; index < data->length; index++)
        {
            data->data[index] = serial_data[3+index];
            data->data[index+1] = '\0';
        }
        status = true;
    }
    return status;
}

bool protocol_create_message(protocol_data_raw_t data, char *serial_data)
{
    bool status = false;
    if (serial_data != NULL)
    {
        serial_data[0] = data.id;
        serial_data[1] = data.action;
        serial_data[2] = data.length;
        for(uint8_t index = 0; index < data.length; index++)
        {
            serial_data[3+index] = data.data[index];
            serial_data[4+index] = '\0';
            serial_data[5+index] = '\n';
        }

        status = true;
    }
    return status;
}

protocol_action_t protocol_get_action(protocol_data_raw_t parsed_data)
{
    return (protocol_action_t)((uint8_t)parsed_data.action&0x01);
}

protocol_address_t protocol_get_address(protocol_data_raw_t parsed_data)
{
    return (protocol_address_t)((uint8_t)parsed_data.action&0xFE>>1);
}

uint8_t protocol_get_length(protocol_data_raw_t parsed_data)
{
    return parsed_data.length;
}

bool protocol_get_data(protocol_data_raw_t parsed_data, char *data)
{
    bool status = false;
    if (parsed_data.length > 0)
    {
        for(uint8_t index = 0; index < parsed_data.length; index++)
        {
            data[index] = parsed_data.data[index];
            data[index+1] = '\0';
        }
    }
    return status;
}

bool protocol_check_id(protocol_data_raw_t data)
{
    bool status = false;
    if (data.id == device_settings.id)
    {
        status = true;
    }
    return status;
}

