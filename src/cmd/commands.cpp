#include <string.h>
#include <stdbool.h>
#include "esp_log.h"
#include "Arduino.h"
#include "esp32-hal-log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

#include "settings/settings.h"
#include "settings/reading.h"

#include "commands_handler.h"
#include "commands_utils.h"
#include "commands.h"

static const char *TAG = "COMMANDS";

struct arg_str  *phone_arg = arg_str0(NULL, NULL, "<telefone>", "Numero do telefone");
struct arg_int  *interval_arg = arg_int0(NULL, "intervalo", "<intervalo>", "Intervalo entre os envios");
struct arg_end  *end     = arg_end(20);

void *phone_argtable[] =    {phone_arg,     end};
void *interval_argtable[] = {interval_arg,  end};

int id_command_handler(int argc, char **argv)
{
    ESP_LOGD(TAG, "Comando 'id' invocado!");
    return 0;
}

int phone_command_handler(int argc, char **argv)
{
    ESP_LOGD(TAG, "Comando 'telefone' invocado!");
    int status = 0;
    int nerrors = arg_parse(argc, argv, phone_argtable);
    if (nerrors != 0)
    {
        arg_print_errors(stdout, end, __func__);
        //arg_print_errors(stderr, phone_args.end, argv[0]);
        status = 1;
        commands_set_response("ERRO");
    }

    if(phone_arg->count > 0)
    {
        ESP_LOGD(TAG, "recebido: %s", phone_arg->sval[0]);
        // Verifica se é um número de telefone
        char phone[30];
        strncpy(phone, phone_arg->sval[0], 30);

        if(command_validate_phone_number(phone))
        {
            if(settings_get_phones_list_length() < MAX_PHONES)
            {
                ESP_LOGD(__func__, "Telefone %s adicionado com sucesso!", phone);
                settings_set_phone(settings_get_phones_list_length(), phone);
                commands_set_response("OK");
                commands_set_update_settings(true);
                status = 0;
            }
            else
            {
                ESP_LOGD(__func__, "Erro ao adicionar o telefone %s.", phone);
                commands_set_response("ERRO");
                status = 1;
            }
        }
    }
    else
    {
        String phones = "";
        for(uint8_t index = 0; index < settings_get_phones_list_length(); index++)
        {
            phones += settings_get_phone(index);
            phones += "\n";
        }
        ESP_LOGD(__func__, "Telefones cadastrados: %s.", phones.c_str());
        commands_set_response(phones.c_str());
    }
    return status;
}

int address_command_handler(int argc, char **argv)
{
    ESP_LOGD(TAG, "Comando 'endereço' invocado!");
    return 0;
}

int interval_command_handler(int argc, char **argv)
{
    ESP_LOGI(TAG, "Comando 'intervalo' invocado!");
    int status = 0;
    int nerrors = arg_parse(argc, argv, phone_argtable);
    if (nerrors != 0)
    {
        ESP_LOGD(__func__, "ERROR");
        arg_print_errors(stdout, end, __func__);
        commands_set_response("ERRO");
        status = 1;
    }
    if(argc > 1)
    {
        int interval = (int)strtol(argv[1], (char**)NULL, 10);
        if(interval > 0)
        {
            ESP_LOGD(__func__, "Intervalo %d recebido", interval);
            settings_set_interval(interval);
            commands_set_update_settings(true);
            commands_set_response("OK");
        }
        else
        {
            commands_set_response("ERRO");
            status = 1;
        }
    }
    else
    {
        ESP_LOGD(__func__, "Nenhum intervalo recebido, enviar o atual! (%d)", settings_get_interval());
        String leitura = String(settings_get_interval());
        commands_set_response(leitura.c_str());
    }
    return status;
}

int reading_command_handler(int argc, char **argv)
{
    ESP_LOGD(TAG, "Comando 'leitura' invocado!");
    String leitura = String(reading_get());
    commands_set_response(leitura.c_str());
    return 0;
}

int commands_init(void)
{   
    esp_console_config_t console_config;
    console_config.max_cmdline_args = 8;
    console_config.max_cmdline_length = 256;
    esp_console_init(&console_config);

    //esp_console_register_help_command();

    const esp_console_cmd_t phone_cmd = {
        .command = "telefone",
        .help = "Lista telefones cadastrados",
        //.help = "Lista telefones cadastrados, cadastra telefone, ou remove se o mesmo já existir",
        .hint = NULL,
        .func = phone_command_handler,
        .argtable = &phone_argtable
    };
    
    const esp_console_cmd_t interval_cmd = {
        .command = "intervalo",
        .help = "Retorna intervalo de envio das leituras",
        //.help = "Retorna intervalo de envio das leituras, ou cadastra novo intervalo se houver argumento",
        .hint = NULL,
        .func = interval_command_handler,
        .argtable = &interval_argtable
    };
    
    const esp_console_cmd_t reading_cmd = {
        .command = "leitura",
        .help = "Retorna as leituras dos dispositivos",
        .hint = NULL,
        .func = reading_command_handler,
    };

    esp_console_cmd_register(&phone_cmd);
    esp_console_cmd_register(&interval_cmd);
    esp_console_cmd_register(&reading_cmd);

    return 0;
}
