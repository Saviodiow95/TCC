#include <Arduino.h>  // Adicionado para Serial
#include <ArduinoJson.h>  // Adicionado para JSON
#include "storage.h"
#include "config.h"   // Para ssid, pass, apiEndereco, apiPorta, feedHour, feedMinute, feedDuration, apitoken

bool iniciarSD() {
  if (!SD.begin(chipSelect)) {
    Serial.println("Erro no SD");
    return false;
  }
  Serial.println("Cartão SD OK");
  return true;
}

void salvarDado(String linha) {
  arquivo = SD.open("dados.csv", FILE_WRITE);
  if (arquivo) {
    arquivo.println(linha);
    arquivo.close();
    Serial.println("Salvo: " + linha);
  } else {
    Serial.println("Erro ao escrever no SD");
  }
}

int calcularPorcentagemEnviados() {
  File temp = SD.open("dados.csv", FILE_READ);
  if (!temp) {
    Serial.println("Erro ao calcular %");
    return 0;
  }

  int total = 0;
  int enviados = 0;

  while (temp.available()) {
    String linha = temp.readStringUntil('\n');
    linha.trim();
    if (linha.length() > 0) {
      total++;
      if (linha.endsWith("true")) {
        enviados++;
      }
    }
  }
  temp.close();

  if (total == 0) return 100;  // Nenhum dado = 100%
  return (enviados * 100) / total;
}

void salvarConfigs() {
  DynamicJsonDocument doc(1024);  // Tamanho suficiente para configs
  doc["ssid"] = ssid;
  doc["pass"] = pass;
  doc["apiEndereco"] = apiEndereco;
  doc["apiPorta"] = apiPorta;
  doc["feedHour"] = feedHour;
  doc["feedMinute"] = feedMinute;
  doc["feedDuration"] = feedDuration;
  doc["apitoken"] = apitoken;  // Novo

  File configFile = SD.open("config.json", FILE_WRITE);
  if (configFile) {
    serializeJson(doc, configFile);
    configFile.close();
    Serial.println("Configs salvas em config.json.");
  } else {
    Serial.println("Erro ao salvar configs. Verifique SD.");
  }
}

void lerConfigs() {
  if (!SD.exists("config.json")) {
    Serial.println("config.json não encontrado. Criando com defaults...");
    salvarConfigs();  // Cria com valores atuais como defaults
    return;
  }

  File configFile = SD.open("config.json", FILE_READ);
  if (configFile) {
    size_t size = configFile.size();
    if (size == 0) {
      Serial.println("config.json vazio. Usando defaults.");
      configFile.close();
      salvarConfigs();
      return;
    }

    String jsonContent = configFile.readString();
    configFile.close();
    Serial.println("Conteúdo do config.json lido: " + jsonContent);  // Debug: Mostra o JSON lido

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, jsonContent);

    if (error) {
      Serial.print("Erro ao parsear JSON: ");
      Serial.println(error.c_str());
      Serial.println("Usando defaults.");
      return;
    }

    // Lê valores, adiciona se missing
    bool modified = false;
    if (doc["ssid"].isNull()) {
      doc["ssid"] = "SavioNetPc";  // Default
      modified = true;
    }
    if (doc["pass"].isNull()) {
      doc["pass"] = "18101995";  // Default
      modified = true;
    }
    if (doc["apiEndereco"].isNull()) {
      doc["apiEndereco"] = "192.168.0.102";  // Default
      modified = true;
    }
    if (doc["apiPorta"].isNull()) {
      doc["apiPorta"] = 8000;  // Default
      modified = true;
    }
    if (doc["feedHour"].isNull()) {
      doc["feedHour"] = 8;  // Default: 8h
      modified = true;
    }
    if (doc["feedMinute"].isNull()) {
      doc["feedMinute"] = 0;  // Default: 00 min
      modified = true;
    }
    if (doc["feedDuration"].isNull()) {
      doc["feedDuration"] = 5;  // Default: 5 seg
      modified = true;
    }
    if (doc["apitoken"].isNull()) {
      doc["apitoken"] = "5f196092e6b3627b65789ddc954c2ded58a00879";  // Novo default
      modified = true;
    }

    strcpy(ssid, doc["ssid"].as<const char*>());
    strcpy(pass, doc["pass"].as<const char*>());
    strcpy(apiEndereco, doc["apiEndereco"].as<const char*>());
    apiPorta = doc["apiPorta"].as<int>();
    feedHour = doc["feedHour"].as<int>();
    feedMinute = doc["feedMinute"].as<int>();
    feedDuration = doc["feedDuration"].as<int>();
    strcpy(apitoken, doc["apitoken"].as<const char*>());  // Novo

    Serial.print("Valores carregados: SSID=");
    Serial.print(ssid);
    Serial.print(", Pass=");
    Serial.print(pass);
    Serial.print(", API=");
    Serial.print(apiEndereco);
    Serial.print(":");
    Serial.print(apiPorta);
    Serial.print(", Feed=");
    Serial.print(feedHour);
    Serial.print(":");
    Serial.print(feedMinute);
    Serial.print(", Duration=");
    Serial.print(feedDuration);
    Serial.print(", Token=");
    Serial.println(apitoken);

    if (modified) {
      salvarConfigs();  // Salva de volta com valores adicionados
      Serial.println("Configs atualizadas com valores default ausentes.");
    }

    Serial.println("Configs lidas de config.json.");
  } else {
    Serial.println("Erro ao abrir config.json. Usando defaults.");
  }
}