/******************
 * STANDARD INCLUDES
 ******************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "commands_test.h"
#include "minunit.h"
/******************
 * INCLUDES TO TEST
 ******************/
#include "../src/protocol/protocol.h"


/******************
 * GLOBAL VARIABLES
 ******************/
uint8_t hello[] = "HELLO\0";
int tests_run;
const char *cmti_response = "+CMTI: \"SM\",15";
const char *cmgr_response = "+CMGR: \"REC UNREAD\",\"+5541998271302\",,\"21/10/05,21:51:45-12\"\nteste";

/******************
 * TEST FUNCTIONS
 ******************/
static char * test_create_message() 
{
    uint8_t data_to_send[MAX_DATA_LENGTH];
    protocol_data_raw_t raw_data_to_send = {
        .id = 0xFF, // to controller
        .action = 0x01, // set - on nothing
        .length = strlen((char *)hello) // this info
    };
    strcpy((char *)raw_data_to_send.data, (char *)hello);
    protocol_create_message(raw_data_to_send, (char *)data_to_send);

    char string[6];
    for (int index = 0; index < raw_data_to_send.length; index++)
    {
        string[index] = data_to_send[3+index];
    }

    mu_assert("error, id != 255", data_to_send[0] == 255);
    mu_assert("error, action != 1", data_to_send[1] == 1);
    mu_assert("error, data length != 5", data_to_send[2] == 5);
    mu_assert("error, data != HELLO", strcmp("HELLO", (char*)string) == 0);
    return 0;
}

static char * test_parse_message_to_device()
{
    uint8_t data_to_be_parsed[] = {0x1, 0x3, 5, 'H', 'E', 'L', 'L', 'O', '\0'};
    protocol_data_raw_t raw_data_from_parse;
    bool status = protocol_message_parse((char *)data_to_be_parsed, &raw_data_from_parse);
    mu_assert("error, protocol_message_parse != true", status);
    return 0;
}

static char * test_parse_message()
{
    uint8_t data_to_be_parsed[] = {0x1, 0x3, 5, 'H', 'E', 'L', 'L', 'O', '\0'};
    protocol_data_raw_t raw_data_from_parse;
    protocol_message_parse((char *)data_to_be_parsed, &raw_data_from_parse);

    mu_assert("error, id != 1", raw_data_from_parse.id == 1);
    mu_assert("error, action != 3", raw_data_from_parse.action == 3);
    printf("length: %ld\n", strlen((char*)raw_data_from_parse.data));
    mu_assert("error, lenght != 5", raw_data_from_parse.length == strlen((char*)raw_data_from_parse.data));
    mu_assert("error, data != HELLO", strcmp("HELLO", (char*)raw_data_from_parse.data) == 0);
    return 0;
}

static char *test_cmti_response()
{
    int slot = 0;
    mu_assert("error, there is no index on response" ,sscanf(cmti_response, "+CMTI: \"SM\",%d", &slot));
    mu_assert("error, index slot != 15", slot==15);
    return 0;
}

static char *test_cmgr_response()
{
    char phone[30], message_status[30], sts[30], date[30], hour[30];
    int ret = sscanf(cmgr_response, "+CMGR: %[^','],%[^','],%[^','],%[^','],%[^',']", message_status, phone, sts, date, hour);
    mu_assert("error, did not found phone", strcmp(phone, "\"+5541998271302\"")==0);
    return 0;
}

/********************
 * ALL TEST FUNCTION
 *******************/
static char * all_tests() 
{
    mu_run_test(test_create_message);
    mu_run_test(test_parse_message_to_device);
    mu_run_test(test_parse_message);
    mu_run_test(test_cmti_response);
    mu_run_test(test_cmgr_response);
    return 0;
}

static bool command_validate_phone_number(const char *phone)
{
    bool status = false;
    int digits = 0;
    char number[15];
    printf( "Verificando se %s é um telefone válido...\n", phone);
    sscanf(phone, " %11[0-9] %n", number, &digits);
    /*
    if (digits == 0)  // no digits
    {
        printf("%s - sem dígitos\n", __func__);
        return false;
    }
    if (number[digits]) // junk after the digits
    {
        printf("%s - dígitos extras\n", __func__);
        return false;
    }
    if (strlen(number) == 11 || strlen(number) == 9)
    {
        return true;
    }
    else
    {
        printf("%s - não tem o tamanho de um número\n", __func__);
        return false; // not 7 digits
    }*/

    if (digits == 0 && number[digits])  // no digits
    {
        status = false;
    }
    else
    {
        if (strlen(number) == 11 || strlen(number) == 9)
        {
            status = true;
        }
        else
        {
            status = false;
        }
    }
    
    return status;
}
/******************
 * MAIN FUNCTION
 ******************/
int main(void)
{
    protocol_init(PERIPHERAL, 1);

    char *result = all_tests();
    if (result != 0) 
    {
        printf("%s\n", result);
    }
    else 
    {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);
    test_commands();

    printf("telefone: %s\n", 
            command_validate_phone_number("04141998271302")?"True":"False");
    printf("telefone: %s\n", 
            command_validate_phone_number("041998271302")?"True":"False");
    printf("telefone: %s\n", 
            command_validate_phone_number("998271302")?"True":"False");
    printf("telefone: %s\n", 
            command_validate_phone_number("+5541998271302")?"True":"False");
    printf("telefone: %s\n", 
            command_validate_phone_number("ok")?"True":"False");
    return result != 0;
}
