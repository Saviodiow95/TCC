#ifndef CONFIG_H
#define CONFIG_H

extern char ssid[32];
extern char pass[32];
extern bool isAPMode;  // Rastreia se está em modo AP
extern char apiEndereco[32];  // Endereço da API
extern int apiPorta;  // Porta da API
extern int feedHour;  // Hora para alimentar (0-23)
extern int feedMinute;  // Minuto para alimentar (0-59)
extern int feedDuration;  // Duração em segundos para ativar o relé
extern char apitoken[41];  // Novo: API token (40 chars + null terminator)

#endif