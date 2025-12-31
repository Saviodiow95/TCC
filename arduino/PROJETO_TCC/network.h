#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <RTClib.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <SD.h>

extern char ssid[];
extern char pass[];
extern RTC_DS1307 rtc;
extern HttpClient http;

void conectarWiFi();
void configurarRTC();
void enviarDadosPendentes();
String gerarPayload(String linha);
bool enviarParaAPI(String payload);
void updateIP();

#endif