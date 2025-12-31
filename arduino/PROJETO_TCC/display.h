#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h> // Para lcd
#include <DallasTemperature.h> // Para sensors
#include <RTClib.h>   // Para DateTime

extern LiquidCrystal_I2C lcd;
extern DallasTemperature sensors;
extern const int pHPin;
extern const int tdsPin;
extern RTC_DS1307 rtc;  // Adicionado para timestamp
extern String ultimoIP;  // Novo: declarado como extern

// Variáveis globais para armazenar últimas leituras (evita reler sensores toda vez)
extern float ultimoPH;
extern float ultimaTemp;
extern float ultimoTDS;
extern int ultimaPorcentagem;
extern String ultimoStatusWiFi;

// Estado da tela para alternar
extern int estadoTela;

void atualizarDisplay();
void lerUltimasLeituras();  // Nova função para atualizar variáveis globais

#endif