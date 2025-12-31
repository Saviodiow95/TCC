#include <Arduino.h>  // Adicionado para Serial, delay, etc.
#include <Wire.h>     // Para I2C
#include "network.h"
#include "display.h"  // Para ultimoIP
#include "config.h"   // Para isAPMode, apitoken

void conectarWiFi() {
  // Reseta modo AP
  isAPMode = false;
  Serial.print("Conectando ao WiFi como cliente...");
  int attempts = 0;
  while (attempts < 3) {  // 3 tentativas
    if (WiFi.begin(ssid, pass) == WL_CONNECTED) {
      Serial.println("\nConectado como cliente!");
      updateIP();
      return;
    }
    delay(5000);
    attempts++;
  }

  // Falhou: Switch para modo AP
  Serial.println("Falha no modo cliente. Iniciando modo AP...");
  isAPMode = true;
  if (WiFi.beginAP("ArduinoConfig", "12345678") == WL_AP_LISTENING) {  // SSID e pass do AP
    delay(1000);
    IPAddress apIP = WiFi.softAPIP();  // IP do AP (tipicamente 192.168.4.1)
    ultimoIP = String(apIP[0]) + "." + String(apIP[1]) + "." + String(apIP[2]) + "." + String(apIP[3]);
    Serial.println("Modo AP ativo. IP: " + ultimoIP);
    Serial.println("Conecte-se à rede 'ArduinoConfig' com senha '12345678' para configurar.");
  } else {
    Serial.println("Falha ao iniciar modo AP.");
    isAPMode = false;  // Evita ficar preso em estado inválido
  }
}

void updateIP() {
  IPAddress ip = WiFi.localIP();
  ultimoIP = String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
  Serial.println("IP obtido: " + ultimoIP);
}

void configurarRTC() {
  Wire.begin();  // Inicializa barramento I2C
  Serial.println("Tentando inicializar RTC no endereço 0x68...");

  // Testa comunicação com o RTC no endereço 0x68
  Wire.beginTransmission(0x68);
  int error = Wire.endTransmission();
  if (error != 0) {
    Serial.print("Erro ao comunicar com RTC (endereço 0x68). Código de erro: ");
    Serial.println(error);
    Serial.println("Verifique conexões (SDA=A4, SCL=A5, VCC=5V, GND) e bateria do RTC.");
    Serial.println("Usando data/hora da compilação como fallback.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    return;  // Continua execução sem travar
  }

  if (!rtc.begin()) {
    Serial.println("RTC não inicializado, mesmo com I2C respondendo. Verifique módulo DS1307.");
    Serial.println("Usando data/hora da compilação como fallback.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    return;
  }

  Serial.println("RTC inicializado com sucesso no endereço 0x68.");
  if (!rtc.isrunning()) {
    Serial.println("RTC parado. Configurando com hora atual...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  } else {
    Serial.println("RTC já está funcionando.");
  }
}

void enviarDadosPendentes() {
  // Verifica se conectado antes de tentar enviar (evita travamento offline)
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Sem conexão WiFi. Pulando envio de dados pendentes.");
    return;  // Sai sem processar, mantém dados no SD para tentar depois
  }

  File temp = SD.open("dados.csv", FILE_READ);
  if (!temp) {
    Serial.println("Erro ao abrir arquivo para leitura");
    return;
  }

  String novoConteudo = "";
  bool dadosEnviados = false;

  while (temp.available()) {
    String linha = temp.readStringUntil('\n');
    linha.trim();

    if (linha.endsWith("false")) {
      String payload = gerarPayload(linha);
      if (enviarParaAPI(payload)) {
        linha.replace("false", "true");
        dadosEnviados = true;
      } else {
        Serial.println("Falha no envio para API. Tentando novamente no próximo ciclo.");
      }
    }
    novoConteudo += linha + "\n";
  }
  temp.close();

  if (dadosEnviados) {
    SD.remove("dados.csv");
    File novoArquivo = SD.open("dados.csv", FILE_WRITE);
    if (novoArquivo) {
      novoArquivo.print(novoConteudo);
      novoArquivo.close();
      Serial.println("Arquivo atualizado com dados marcados como enviados.");
    }
  }
}

String gerarPayload(String linha) {
  // timestamp,pH,temp,tds,nivel_racao,false
  int pos1 = linha.indexOf(",");
  int pos2 = linha.indexOf(",", pos1 + 1);
  int pos3 = linha.indexOf(",", pos2 + 1);
  int pos4 = linha.indexOf(",", pos3 + 1);
  int pos5 = linha.indexOf(",", pos4 + 1);

  String horario = linha.substring(0, pos1);  // Atualizado: timestamp para horario
  String ph = linha.substring(pos1 + 1, pos2);
  String temperatura = linha.substring(pos2 + 1, pos3);
  String codutividade = linha.substring(pos3 + 1, pos4);  // Atualizado para codutividade
  String nivelRacao = linha.substring(pos4 + 1, pos5);

  return "{\"horario\":\"" + horario + "\",\"ph\":" + ph + ",\"temperatura\":" + temperatura + ",\"codutividade\":" + codutividade + ",\"nivel_racao\":" + nivelRacao + "}";
}

bool enviarParaAPI(String payload) {
  http.beginRequest();
  http.post("/api/historico/");  // Seu endpoint real
  http.sendHeader("Content-Type", "application/json");
  http.sendHeader("Content-Length", payload.length());
  http.sendHeader("Authorization", "Token " + String(apitoken));  // Novo: autenticação com token
  http.beginBody();
  http.print(payload);
  http.endRequest();

  int status = http.responseStatusCode();
  String resp = http.responseBody();

  Serial.print("Resposta da API (");
  Serial.print(status);
  Serial.println("): " + resp);

  return (status == 200 || status == 201);  // Aceita 200 ou 201 para sucesso
}