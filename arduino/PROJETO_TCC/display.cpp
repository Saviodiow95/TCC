#include <Arduino.h>  // Adicionado para analogRead, WiFi, Serial, etc.
#include <WiFiS3.h>   // Para WiFi
#include <math.h>     // Adicionado para pow()
#include "display.h"
#include "sensors.h"  // Para calibrarPH
#include "storage.h"  // Para calcularPorcentagemEnviados

// Definição das variáveis globais
float ultimoPH = 0.0;
float ultimaTemp = 0.0;
float ultimoTDS = 0.0;
int ultimaPorcentagem = 0;
String ultimoStatusWiFi = "OFF";
String ultimoIP = "N/A";  // Novo: default se não conectado
int estadoTela = 0;  // 0: Tela 1, 1: Tela 2, 2: Tela 3, 3: Tela 4 (IP)

void lerUltimasLeituras() {
  sensors.requestTemperatures();
  ultimaTemp = sensors.getTempCByIndex(0);
  int analogValuePH = analogRead(pHPin);
  float voltagePH = analogValuePH * (5.0 / 1024.0);
  ultimoPH = calibrarPH(voltagePH);
  int analogValueTDS = analogRead(tdsPin);
  float voltageTDS = analogValueTDS * (5.0 / 1024.0);
  float compensationVoltage = voltageTDS / (1.0 + 0.02 * (ultimaTemp - 25.0));
  ultimoTDS = (133.42 * pow(compensationVoltage, 3) - 255.86 * pow(compensationVoltage, 2) + 857.39 * compensationVoltage) * 0.5;

  ultimoStatusWiFi = (WiFi.status() == WL_CONNECTED) ? "OK" : "OFF";
  ultimaPorcentagem = calcularPorcentagemEnviados();
}

void atualizarDisplay() {
  // Atualiza leituras apenas se necessário
  lerUltimasLeituras();

  lcd.clear();

  // Alterna entre telas
  switch (estadoTela) {
    case 0:  // Tela 1: pH e Temperatura
      lcd.setCursor(0, 0);
      lcd.print("pH: " + String(ultimoPH, 1));
      lcd.setCursor(0, 1);
      lcd.print("Temp: " + String(ultimaTemp, 1) + " C");
      break;
    case 1:  // Tela 2: TDS e Porcentagem
      lcd.setCursor(0, 0);
      lcd.print("TDS: " + String((int)ultimoTDS) + " ppm");
      lcd.setCursor(0, 1);
      lcd.print("Dados: " + String(ultimaPorcentagem) + "%");
      break;
    case 2: {  // Tela 3: WiFi e Hora
      DateTime agora = rtc.now();
      String hora = (agora.hour() < 10 ? "0" : "") + String(agora.hour()) + ":" +
                    (agora.minute() < 10 ? "0" : "") + String(agora.minute());
      lcd.setCursor(0, 0);
      lcd.print("WiFi: " + ultimoStatusWiFi);
      lcd.setCursor(0, 1);
      lcd.print("Hora: " + hora);
      break;
    }
    case 3:  // Tela 4: IP do Arduino
      lcd.setCursor(0, 0);
      lcd.print("IP Arduino:");
      lcd.setCursor(0, 1);
      lcd.print(ultimoIP);
      break;
  }

  estadoTela = (estadoTela + 1) % 4;
}