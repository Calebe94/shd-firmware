#ifndef _READINGS_H_
#define _READINGS_H_

#define READING_FILE    "leitura.json"

/*
 * Funções de controle e manuseio das informações de leitura do dispositivo(controlador e perfiférico).
 *
 * */
void reading_load();

void reading_update();

float reading_get();

void reading_set(float);

#endif
