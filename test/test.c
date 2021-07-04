#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../src/protocol/protocol.h"

uint8_t hello[] = "HELLO\0";

int main(void)
{
    uint8_t data_to_send[MAX_DATA_LENGTH];
    
    protocol_init(SLAVE, 1);
    protocol_data_raw_t raw_data_to_send = {
        .id = 0xFF, // to master
        .action = 0x01, // set - on nothing
        .length = strlen((char *)hello) // this info
    };
    strcpy((char *)raw_data_to_send.data, (char *)hello);
    protocol_create_message(raw_data_to_send, (char *)data_to_send);
    
    printf("From Struct:\n");
    printf("id: %x - action: %x - length: %d - data: %s\n", 
            raw_data_to_send.id, raw_data_to_send.action, raw_data_to_send.length, 
            (char*)raw_data_to_send.data);

    char string[6];
    for (int index = 0; index < raw_data_to_send.length; index++)
    {
        string[index] = data_to_send[3+index];
    }
    printf("From string:\n");
    printf("id: %x - action: %x - length: %d - data: %s\n", 
            data_to_send[0],
            data_to_send[1],
            data_to_send[2], string);
    // ------------------------------------

    printf("----------------------------\n");
    uint8_t data_to_be_parsed[] = {0x1, 0x3, 5, 'H', 'E', 'L', 'L', 'O', '\0'};
    protocol_data_raw_t raw_data_from_parse;
    bool status = protocol_message_parse((char *)data_to_be_parsed, &raw_data_from_parse);
    printf("Status do protocol_message_parse: %s\n", status?"TRUE":"FALSE");
    printf("%s\n", data_to_be_parsed);
    printf("From Struct parsed:\n");
    printf("id: %x - action: %x - length: %d - data: %s\n", 
            raw_data_from_parse.id, raw_data_from_parse.action, raw_data_from_parse.length, 
            (char*)raw_data_from_parse.data);

    return 0;
}
