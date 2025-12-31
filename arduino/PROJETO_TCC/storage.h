#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>  // Adicionado para String e funções core
#include <SD.h>       // Para SD e File

extern const int chipSelect;
extern File arquivo;

bool iniciarSD();
void salvarDado(String linha);
int calcularPorcentagemEnviados();
void salvarConfigs();
void lerConfigs();

#endif