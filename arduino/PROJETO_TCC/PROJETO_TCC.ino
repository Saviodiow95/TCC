#include <Arduino.h>  // Adicionado para funções core como Serial, delay, etc.
#include <WiFiS3.h>
#include <RTClib.h>
#include <SPI.h>
#include <SD.h>
#include <ArduinoHttpClient.h>
#include <OneWire.h>          // Para DS18B20
#include <DallasTemperature.h> // Para DS18B20
#include <math.h>             // Para pow() no cálculo de TDS
#include <LiquidCrystal_I2C.h> // Para LCD 16x2 I2C
#include <Thread.h>           // Para ArduinoThread
#include <ThreadController.h> // Para gerenciar threads
#include <typeinfo>

#include "sensors.h"
#include "display.h"
#include "storage.h"
#include "network.h"
#include "webserver.h"        // Para servidor web
#include "config.h"           // Adicionado para ssid, pass, isAPMode, etc.
#include "actuators.h"        // Para lerNivelRacao e dispensarRacao

//// OBJETOS ////
RTC_DS1307 rtc;
WiFiClient wifiClient;
HttpClient http(wifiClient, apiEndereco, apiPorta);  // Atualizado para usar variáveis

const int chipSelect = 10;
File arquivo;

OneWire oneWire(2);  // Pino digital para DS18B20
DallasTemperature sensors(&oneWire);

const int pHPin = A0;  // Pino analógico para sensor de pH 4502C
const int tdsPin = A1; // Pino analógico para sensor TDS Meter V1.0

LiquidCrystal_I2C lcd(0x3F, 16, 2);  // Endereço I2C corrigido para 0x3F

// Threads para tarefas simultâneas
Thread threadLerSensores = Thread();
Thread threadAtualizarDisplay = Thread();
Thread threadEnviarDados = Thread();
Thread threadAlimentador = Thread();  // Novo: thread para verificar horário e ativar alimentador
ThreadController controller = ThreadController();  // Gerenciador de threads

//// SETUP ////
void setup() {
  Serial.begin(9600);
  delay(1000);

  lcd.init();      // Inicia o LCD
  lcd.backlight(); // Liga backlight

  // Inicializa SD antes de ler configs
  if (!iniciarSD()) {
    Serial.println("Falha ao inicializar SD. Verifique o cartão.");
  }

  // Lê configs do SD após SD inicializado
  lerConfigs();

  // ReInicializa HttpClient com possíveis novos valores de configs
  http = HttpClient(wifiClient, apiEndereco, apiPorta);

  conectarWiFi();
  configurarRTC();
  sensors.begin();  // Inicia DS18B20

  // Novo: Inicializa pins do alimentador
  setupActuators();

  // Teste inicial: gera um dado de exemplo (remova após testes)
  gerarDadoExemplo();

  // Mensagem inicial no LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Inicializando...");
  delay(2000);

  // Inicializa leituras e display
  lerUltimasLeituras();
  atualizarDisplay();  // Chama uma vez para forçar a primeira tela

  // Configura threads
  threadLerSensores.onRun(lerESalvarSensores);
  threadLerSensores.setInterval(1800000);  // 30 minutos para ler e salvar sensores

  threadAtualizarDisplay.onRun(atualizarDisplay);
  threadAtualizarDisplay.setInterval(3000);  // 3 seg para atualizar display

  threadEnviarDados.onRun(enviarDadosPendentes);
  threadEnviarDados.setInterval(10000);  // 10 minutos para enviar dados
  // threadEnviarDados.setInterval(600000);  // 10 minutos para enviar dados

  // Novo: Thread para alimentador (verifica a cada 10 seg para evitar perder o horário)
  threadAlimentador.onRun(verificarAlimentador);
  threadAlimentador.setInterval(10000);  // 10 segundos

  // Adiciona threads ao controller
  controller.add(&threadLerSensores);
  controller.add(&threadAtualizarDisplay);
  controller.add(&threadEnviarDados);
  controller.add(&threadAlimentador);  // Novo

  // Inicializa servidor web
  setupWebServer();
}

//// LOOP ////
void loop() {
  // Executa as threads de forma não bloqueante
  controller.run();

  // Lida com requisições web (non-blocking)
  handleWebServer();

  // Verifica e reconecta WiFi apenas se NÃO estiver em modo AP
  if (!isAPMode && WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado. Reconectando...");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Perdido...");
    lcd.setCursor(0, 1);
    lcd.print("Tentando reconex.");
    conectarWiFi();
  }
}

// Novo: Função para verificar horário e ativar alimentador
void verificarAlimentador() {
  DateTime now = rtc.now();
  Serial.print("Verificando alimentador: Hora atual ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(", Configurado ");
  Serial.print(feedHour);
  Serial.print(":");
  Serial.println(feedMinute);


  if (now.hour() == feedHour && now.minute() == feedMinute) {
    dispensarRacao();  // Ativa o relé por feedDuration segundos
  }
}