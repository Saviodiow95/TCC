#include <Arduino.h>  // Adicionado para Serial, delayMicroseconds, pulseIn, etc.
#include "actuators.h"
#include "config.h"   // Para feedDuration

const int trigPin = 3;  // Pino trig do AJ-SR04M
const int echoPin = 4;  // Pino echo do AJ-SR04M
const int relePin = 5;  // Pino do relé para motor

const float comprimentoCano = 52.0;  // cm

void setupActuators() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relePin, OUTPUT);
  digitalWrite(relePin, LOW);  // Desliga inicialmente (agora active high)
}

float lerNivelRacao() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distancia = duration * 0.034 / 2;  // cm (velocidade som 0.034 cm/us)

  float nivelPreenchido = comprimentoCano - distancia; // cm preenchidos
  float porcentagem = (nivelPreenchido / comprimentoCano) * 100.0;
  if (porcentagem < 0) porcentagem = 0;
  if (porcentagem > 100) porcentagem = 100;

  return porcentagem;
}

void dispensarRacao() {
  digitalWrite(relePin, HIGH);  // Liga motor (assumindo active high)
  delay(feedDuration * 1000);  // Usa feedDuration em segundos
  digitalWrite(relePin, LOW);  // Desliga
  Serial.println("Racao dispensada por " + String(feedDuration) + " segundos.");
}