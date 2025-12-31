#ifndef PAGES_H
#define PAGES_H

#include <WiFiS3.h>

void handleRoot(WiFiClient& client);
void handleDados(WiFiClient& client);
void handleConfigGet(WiFiClient& client);
void handleConfigPost(WiFiClient& client, String body);

#endif