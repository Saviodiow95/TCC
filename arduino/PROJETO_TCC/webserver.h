#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <Arduino.h>
#include <WiFiS3.h>     // Para WiFiServer

extern char ssid[];  // Para configs
extern char pass[];

void setupWebServer();
void handleWebServer();
void handleRoot(WiFiClient& client);
void handleDados(WiFiClient& client);
void handleConfigGet(WiFiClient& client);
void handleConfigPost(WiFiClient& client, String body);

#endif