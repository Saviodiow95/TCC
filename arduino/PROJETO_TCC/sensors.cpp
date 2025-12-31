#include <Arduino.h>  // Adicionado para Serial, random, analogRead, etc.
#include <math.h>     // Adicionado para pow()
#include "sensors.h"
#include "storage.h"  // Para salvarDado
#include "display.h"  // Para lerUltimasLeituras
#include "actuators.h"  // Novo: Para lerNivelRacao e dispensarRacao

void gerarDadoExemplo() {
  DateTime agora = rtc.now();
  String timestamp = String(agora.year(), DEC) + "-" + 
                     (agora.month() < 10 ? "0" : "") + String(agora.month(), DEC) + "-" + 
                     (agora.day() < 10 ? "0" : "") + String(agora.day(), DEC) + " " + 
                     (agora.hour() < 10 ? "0" : "") + String(agora.hour(), DEC) + ":" + 
                     (agora.minute() < 10 ? "0" : "") + String(agora.minute(), DEC) + ":" + 
                     (agora.second() < 10 ? "0" : "") + String(agora.second(), DEC);

  float pH = random(60, 80) / 10.0;      // Fictício: 6.0 a 8.0
  float temp = random(200, 300) / 10.0;  // Fictício: 20.0 a 30.0
  float tds = random(100, 500);          // Fictício: 100 a 500 ppm
  float nivelRacao = random(0, 100);     // Fictício: 0 a 100%

  String linha = timestamp + "," + String(pH, 2) + "," + String(temp, 2) + "," + String(tds, 2) + "," + String(nivelRacao, 2) + ",false";
  salvarDado(linha);
}

void lerESalvarSensores() {
  DateTime agora = rtc.now();
  String timestamp = String(agora.year(), DEC) + "-" + 
                     (agora.month() < 10 ? "0" : "") + String(agora.month(), DEC) + "-" + 
                     (agora.day() < 10 ? "0" : "") + String(agora.day(), DEC) + " " + 
                     (agora.hour() < 10 ? "0" : "") + String(agora.hour(), DEC) + ":" + 
                     (agora.minute() < 10 ? "0" : "") + String(agora.minute(), DEC) + ":" + 
                     (agora.second() < 10 ? "0" : "") + String(agora.second(), DEC);

  // Lê temperatura DS18B20
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);  // Assume um sensor; -127 indica erro
  if (temp == -127.00) {
    Serial.println("Erro na leitura de temperatura!");
    temp = 0.0;  // Valor default
  }

  // Lê pH 4502C (calibração linear simples; ajuste com testes reais)
  int analogValuePH = analogRead(pHPin);
  float voltagePH = analogValuePH * (5.0 / 1024.0);  // Referência 5V
  float pH = calibrarPH(voltagePH);

  // Lê TDS Meter V1.0 com compensação de temperatura
  int analogValueTDS = analogRead(tdsPin);
  float voltageTDS = analogValueTDS * (5.0 / 1024.0);
  float compensationVoltage = voltageTDS / (1.0 + 0.02 * (temp - 25.0));  // Compensa para 25°C
  float tds = (133.42 * pow(compensationVoltage, 3) - 255.86 * pow(compensationVoltage, 2) + 857.39 * compensationVoltage) * 0.5;
  // Opcional: Ajuste com fator de calibração (ex: tds *= 1.0; teste com solução padrão)

  // Novo: Lê nível de ração
  float nivelRacao = lerNivelRacao();
  if (nivelRacao < 20.0) {  // Exemplo: dispensa se < 20%
    dispensarRacao();
  }

  String linha = timestamp + "," + String(pH, 2) + "," + String(temp, 2) + "," + String(tds, 2) + "," + String(nivelRacao, 2) + ",false";
  salvarDado(linha);
}

float calibrarPH(float voltage) {
  // Calibração linear exemplo: Ajuste com soluções conhecidas (pH 4.0 e 7.0)
  // Fórmula: pH = a * voltage + b
  float a = -5.70;  // Slope (exemplo)
  float b = 21.34;  // Offset
  return a * voltage + b;
}