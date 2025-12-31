#include "webserver.h"
#include "network.h"  // Para conectarWiFi após configs
#include "storage.h"  // Para SD em handleDados e salvarConfigs
#include "config.h"   // Para isAPMode

WiFiServer server(80);  // Servidor na porta 80

void setupWebServer() {
  server.begin();
  Serial.println("Servidor web iniciado.");
}

void handleWebServer() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    String currentLine = "";  // Para ler a requisição linha a linha
    String method = "";
    String url = "";
    String body = "";  // Para POST
    boolean isPost = false;
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        currentLine += c;
        if (c == '\n') {
          if (currentLine.startsWith("GET ") || currentLine.startsWith("POST ")) {
            isPost = currentLine.startsWith("POST ");
            int space1 = currentLine.indexOf(' ');
            int space2 = currentLine.indexOf(' ', space1 + 1);
            method = currentLine.substring(0, space1);
            url = currentLine.substring(space1 + 1, space2);
            if (url == "" || url == " ") url = "/";  // Novo: Trata path vazio como root
          }
          if (currentLineIsBlank) {
            // Fim da headers, lê body se POST
            if (isPost) {
              while (client.available()) {
                body += (char)client.read();
              }
            }
            // Processa a rota baseada na URL
            if (url == "/") {
              handleRoot(client);
            } else if (url == "/dados") {
              handleDados(client);
            } else if (url == "/config") {
              if (isPost) {
                handleConfigPost(client, body);
              } else {
                handleConfigGet(client);
              }
            } else {
              client.println("HTTP/1.1 404 Not Found");
              client.println("Content-Type: text/html");
              client.println();
              client.println("<h1>404 Not Found</h1>");
            }
            break;
          }
          if (c == '\n') {
            currentLineIsBlank = true;
            currentLine = "";
          } else if (c != '\r') {
            currentLineIsBlank = false;
          }
        }
      }
    }
    delay(1);  // Tempo para browser receber dados
    client.stop();
    Serial.println("client disconnected");
  }
}