#include "esp_log.h"
#include "esp32-hal-log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

#include "commands.h"

static const char *TAG = "COMMANDS";

struct arg_str  *phone_arg = arg_str0(NULL, NULL, "<telefone>", "Numero do telefone");
struct arg_end  *end     = arg_end(20);
void* phone_argtable[] = {phone_arg, end};

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
    }

    if(phone_arg->count > 0)
    {
        ESP_LOGD(TAG, "recebido: %s", phone_arg->sval[0]);
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
    return 0;
}

int reading_command_handler(int argc, char **argv)
{
    ESP_LOGD(TAG, "Comando 'leitura' invocado!");
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
