#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>  // Adicionado para String e funções core
#include <RTClib.h>   // Para DateTime
#include <DallasTemperature.h> // Para sensors
#include <OneWire.h>  // Para OneWire

extern RTC_DS1307 rtc;
extern DallasTemperature sensors;
extern const int pHPin;  // Pino analógico para sensor de pH 4502C
extern const int tdsPin; // Pino analógico para sensor TDS Meter V1.0

void gerarDadoExemplo();
void lerESalvarSensores();
float calibrarPH(float voltage);

#endif