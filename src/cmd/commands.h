#ifndef _COMMANDS_H_
#define _COMMANDS_H_
#include "esp_console.h"
#include "argtable3/argtable3.h"

/*****************
 *  Commands Data
 ****************/

/******************
 * id command data
 ******************/
typedef struct {
    struct arg_str *id;
    struct arg_end *end;
} settings_id_args_t;

/*********************
 * phone command data
 ********************/
typedef struct {
    struct arg_str *phone;
    struct arg_end *end;
} settings_phone_args_t;

/***********************
 * address command data
 **********************/
typedef struct {
    struct arg_str *address;
    struct arg_end *end;
} settings_address_args_t;

/************************
 * interval command data
 ***********************/
typedef struct {
    struct arg_str *interval;
    struct arg_end *end;
} settings_interval_args_t;


/*
 * Comando: id
 *
 * Exemplos: 
 *
 * id 1: define o id como "1";
 *
 * id: retorna o id configurado;
 * */
int id_command_handler(int, char**);

/*
 * Comando: telefone
 *
 * Exemplos: 
 *
 * telefone: retorna a lista de telefones cadastrados;
 *
 * telefone +5540999999999: se este telefone já estiver configurado, remove da lista, se não adiciona;
 * */
int phone_command_handler(int, char**);

/*
 * Comando: endereço
 *
 * Exemplos: 
 *
 * endereço: retorna a endereço cadastrado;
 *
 * endereço "nome do novo local": cadastra o novo endereço;
 * */
int address_command_handler(int, char**);

/*
 * Comando: intervalo
 *
 * Exemplos: 
 *
 * intervalo: retorna o intervalo cadastrado;
 *
 * intervalo 30: cadastra o novo intervalo(em minutos);
 * */
int interval_command_handler(int, char**);

/*
 * Comando: leituras
 *
 * Exemplos: 
 *
 * leitura: retorna todas as leituras;
 * */
int reading_command_handler(int, char**);

/*
 * Inicializa e registra todos os comandos
 * */
int commands_init(void);

#endif
