#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>

extern const int trigPin;  // Pino trig para AJ-SR04M
extern const int echoPin;  // Pino echo para AJ-SR04M
extern const int relePin;  // Pino relé para motor

void setupActuators();  // Inicializa pins
float lerNivelRacao();  // Lê nível de ração
void dispensarRacao();  // Aciona motor via relé

#endif