#include "pages.h"
#include "network.h"  // Para conectarWiFi após configs
#include "storage.h"  // Para SD em handleDados e salvarConfigs
#include "config.h"   // Para isAPMode, apiEndereco, apiPorta, feedHour, feedMinute, feedDuration, apitoken

void handleRoot(WiFiClient& client) {
  String html = "<html><head>"
                "<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css' rel='stylesheet' integrity='sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM' crossorigin='anonymous'>"
                "<script src='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js' integrity='sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz' crossorigin='anonymous'></script>"
                "<style>body {background: #f0f0f0; color: #333;} .sidebar {position: fixed; top: 0; bottom: 0; left: 0; z-index: 100; padding: 48px 0 0; box-shadow: inset -1px 0 0 rgba(0, 0, 0, .1);} .sidebar-sticky {position: relative; top: 0; height: calc(100vh - 48px); padding-top: .5rem; overflow-x: hidden; overflow-y: auto;}</style>"
                "</head><body>"
                "<div class='d-flex' id='wrapper'>"
                "<div class='bg-light border-right' id='sidebar-wrapper'>"
                "<div class='sidebar-heading'>Menu</div>"
                "<div class='list-group list-group-flush sidebar-sticky'>"
                "<a href='/dados' class='list-group-item list-group-item-action bg-light'>Ver Dados</a>"
                "<a href='/config' class='list-group-item list-group-item-action bg-light'>Configuracoes</a>"
                "</div></div>"
                "<div id='page-content-wrapper' class='container-fluid text-center d-flex align-items-center justify-content-center' style='height: 100vh;'>"
                "<h1>Bem-vindo ao Sistema Aquaponico</h1>"
                "</div></div></body></html>";
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.print(html);
}

void handleDados(WiFiClient& client) {
  String html = "<html><head>"
                "<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css' rel='stylesheet' integrity='sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM' crossorigin='anonymous'>"
                "<script src='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js' integrity='sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz' crossorigin='anonymous'></script>"
                "<style>body {background: #f0f0f0; color: #333;} table {width: 100%;} .back-btn {margin-bottom: 20px;}</style>"
                "</head><body class='container'>"
                "<a href='/' class='btn btn-primary back-btn'>Voltar para Inicio</a>"
                "<h1 class='text-center my-4'>Dados Salvos</h1><table class='table table-bordered table-striped'><thead class='table-dark'><tr><th>Data/Hora</th><th>pH</th><th>Temperatura (°C)</th><th>Condutividade (ppm)</th><th>Nível Ração (%)</th><th>Enviado</th></tr></thead><tbody>";
  File file = SD.open("dados.csv", FILE_READ);
  if (file) {
    while (file.available()) {
      String linha = file.readStringUntil('\n');
      linha.trim();
      if (linha.length() > 0) {
        html += "<tr>";
        int pos1 = linha.indexOf(",");
        int pos2 = linha.indexOf(",", pos1 + 1);
        int pos3 = linha.indexOf(",", pos2 + 1);
        int pos4 = linha.indexOf(",", pos3 + 1);
        int pos5 = linha.indexOf(",", pos4 + 1);
        int pos6 = linha.indexOf(",", pos5 + 1);

        String timestamp = linha.substring(0, pos1);
        // Formata data para padrão brasileiro DD/MM/YYYY HH:MM:SS
        int yearPos = timestamp.indexOf("-");
        int monthPos = timestamp.indexOf("-", yearPos + 1);
        int dayPos = timestamp.indexOf(" ", monthPos + 1);
        int hourPos = timestamp.indexOf(":", dayPos + 1);
        int minPos = linha.indexOf(":", hourPos + 1);
        int secPos = linha.length();

        String year = timestamp.substring(0, yearPos);
        String month = timestamp.substring(yearPos + 1, monthPos);
        String day = timestamp.substring(monthPos + 1, dayPos);
        String hour = timestamp.substring(dayPos + 1, hourPos);
        String min = linha.substring(hourPos + 1, minPos);
        String sec = linha.substring(minPos + 1, secPos);

        String formattedTimestamp = day + "/" + month + "/" + year + " " + hour + ":" + min + ":" + sec;

        String pH = linha.substring(pos1 + 1, pos2);
        String temp = linha.substring(pos2 + 1, pos3);
        String tds = linha.substring(pos3 + 1, pos4);
        String nivelRacao = linha.substring(pos4 + 1, pos5);
        String enviado = linha.substring(pos5 + 1, pos6);

        // Convert boolean to "Sim"/"Não"
        String enviadoStr = (enviado == "true") ? "Sim" : "Não";

        html += "<td>" + formattedTimestamp + "</td>";
        html += "<td>" + pH + "</td>";
        html += "<td>" + temp + " °C</td>";
        html += "<td>" + tds + " ppm</td>";
        html += "<td>" + nivelRacao + " %</td>";
        html += "<td>" + enviadoStr + "</td>";
        html += "</tr>";
      }
    }
    file.close();
  } else {
    html += "<tr><td colspan='6'>Erro ao ler dados</td></tr>";
  }
  html += "</tbody></table></body></html>";
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.print(html);
}

void handleConfigGet(WiFiClient& client) {
  String html = "<html><head><meta charset='UTF-8'>"
                "<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css' rel='stylesheet' integrity='sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM' crossorigin='anonymous'>"
                "<script src='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js' integrity='sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz' crossorigin='anonymous'></script>"
                "<style>body {background: #f0f0f0; color: #333;} .config-form {max-width: 500px; margin: auto; padding: 20px; background: white; border-radius: 8px; box-shadow: 0 0 10px rgba(0,0,0,0.1);} .back-btn {margin-bottom: 20px;}</style>"
                "</head><body class='container'>"
                "<a href='/' class='btn btn-primary back-btn'>Voltar para Inicio</a>"
                "<h1 class='text-center my-4'>Configuracoes</h1>"
                "<form method='POST' class='config-form'>"
                "<div class='mb-3'><label class='form-label'>SSID</label><input class='form-control' name='ssid' value='" + String(ssid) + "'></div>"
                "<div class='mb-3'><label class='form-label'>Senha</label><input class='form-control' name='pass' value='" + String(pass) + "'></div>"
                "<div class='mb-3'><label class='form-label'>Endereço da API</label><input class='form-control' name='apiEndereco' value='" + String(apiEndereco) + "'></div>"
                "<div class='mb-3'><label class='form-label'>Porta da API</label><input class='form-control' name='apiPorta' value='" + String(apiPorta) + "' type='number'></div>"
                "<div class='mb-3'><label class='form-label'>Hora para Alimentar (0-23)</label><input class='form-control' name='feedHour' value='" + String(feedHour) + "' type='number' min='0' max='23'></div>"
                "<div class='mb-3'><label class='form-label'>Minuto para Alimentar (0-59)</label><input class='form-control' name='feedMinute' value='" + String(feedMinute) + "' type='number' min='0' max='59'></div>"
                "<div class='mb-3'><label class='form-label'>Duração do Alimentador (segundos)</label><input class='form-control' name='feedDuration' value='" + String(feedDuration) + "' type='number' min='1' max='60'></div>"
                "<button type='submit' class='btn btn-success w-100'>Salvar</button></form></body></html>";
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.print(html);
}

void handleConfigPost(WiFiClient& client, String body) {
  // Parse body simples (ex: ssid=valor&pass=valor&apiEndereco=valor&apiPorta=valor&feedHour=valor&feedMinute=valor&feedDuration=valor)
  int ssidPos = body.indexOf("ssid=");
  int passPos = body.indexOf("&pass=");
  int apiEnderecoPos = body.indexOf("&apiEndereco=");
  int apiPortaPos = body.indexOf("&apiPorta=");
  int feedHourPos = body.indexOf("&feedHour=");
  int feedMinutePos = body.indexOf("&feedMinute=");
  int feedDurationPos = body.indexOf("&feedDuration=");

  if (ssidPos != -1 && passPos != -1 && apiEnderecoPos != -1 && apiPortaPos != -1 && feedHourPos != -1 && feedMinutePos != -1 && feedDurationPos != -1) {
    String newSsid = body.substring(ssidPos + 5, passPos);
    String newPass = body.substring(passPos + 6, apiEnderecoPos);
    String newApiEndereco = body.substring(apiEnderecoPos + 13, apiPortaPos);
    String newApiPortaStr = body.substring(apiPortaPos + 10, feedHourPos);
    String newFeedHourStr = body.substring(feedHourPos + 10, feedMinutePos);
    String newFeedMinuteStr = body.substring(feedMinutePos + 12, feedDurationPos);
    String newFeedDurationStr = body.substring(feedDurationPos + 14);

    strcpy(ssid, newSsid.c_str());
    strcpy(pass, newPass.c_str());
    strcpy(apiEndereco, newApiEndereco.c_str());
    apiPorta = newApiPortaStr.toInt();
    feedHour = newFeedHourStr.toInt();
    feedMinute = newFeedMinuteStr.toInt();
    feedDuration = newFeedDurationStr.toInt();

    salvarConfigs();  // Salva no SD
    isAPMode = false; // Força tentativa de modo cliente
    conectarWiFi();   // Reconecta
  }
  String html = "<html><head>"
                "<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css' rel='stylesheet' integrity='sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM' crossorigin='anonymous'>"
                "<script src='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js' integrity='sha384-geWF76RCwLtnZ8qwWowPQNguL3RmwHVBC9FhGdlKrxdiJJigb/j/68SIy3Te4Bkz' crossorigin='anonymous'></script>"
                "<style>body {background: #f0f0f0; color: #333;}</style>"
                "</head><body class='container text-center my-5'><h1>Configuracoes salvas! Reiniciando WiFi...</h1>"
                "<a href='/' class='btn btn-primary'>Voltar para Inicio</a></body></html>";
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.print(html);
}