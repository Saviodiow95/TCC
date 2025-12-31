#include <Arduino.h>  // Para String e tipos
#include "config.h"   // Para declarações extern

// Definições das variáveis (somente aqui para evitar múltiplas definições)
char ssid[32] = "SavioNetPc";
char pass[32] = "18101995";
bool isAPMode = false;
char apiEndereco[32] = "192.168.0.102";
int apiPorta = 8000;
int feedHour = 8;
int feedMinute = 0;
int feedDuration = 5;
char apitoken[41] = "04307f3b0011299ef33b235d29cfd1bb35c4f434";  // Novo token padrão atualizado