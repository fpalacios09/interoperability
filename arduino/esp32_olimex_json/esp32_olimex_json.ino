#include <ETH.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

// Configuración Ethernet (Olimex ESP32-POE)
#define ETH_ADDR        0
#define ETH_POWER_PIN   12
#define ETH_MDC_PIN     23
#define ETH_MDIO_PIN    18
#define ETH_TYPE        ETH_PHY_LAN8720
#define ETH_CLK_MODE    ETH_CLOCK_GPIO17_OUT

// Configuración IP estática
IPAddress local_IP(192, 168, 1, 102);  // Cambia según tu dispositivo
IPAddress subnet(255, 255, 255, 0);
IPAddress gateway(192, 168, 1, 100);
IPAddress primaryDNS(8, 8, 8, 8);

// Configuración del servidor
const char* serverURL = "http://192.168.1.100:5000/api/devices/register";
const char* deviceID = "esp32-olimex-01";  // Cambia esto por un ID único
const char* hardware = "ESP32-OLIMEX-PoE";

bool announced = false;

void setup() {
  Serial.begin(115200);
  
  // Inicia Ethernet
  ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE);
  
  // Configura IP estática
  if (!ETH.config(local_IP, gateway, subnet, primaryDNS)) {
    Serial.println("Error al configurar IP estática");
  }

  // Espera conexión Ethernet
  while (!ETH.linkUp()) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n¡Conectado por Ethernet!");
  Serial.print("IP estática: ");
  Serial.println(ETH.localIP());

  // Enviar anuncio una sola vez
  sendAnnouncement();
}

void loop() {
  // Nada que hacer aquí, el dispositivo solo se anuncia al inicio
}

void sendAnnouncement() {
  if (announced) return;
  
  HTTPClient http;
  
  // Crear JSON con la información del dispositivo
  DynamicJsonDocument doc(256);
  doc["id"] = deviceID;
  doc["ip"] = ETH.localIP().toString();
  doc["hardware"] = hardware;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  // Enviar datos al servidor
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");
  
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode > 0) {
    Serial.print("Anuncio enviado. HTTP Response code: ");
    Serial.println(httpResponseCode);
    announced = true;
  } else {
    Serial.print("Error en el anuncio: ");
    Serial.println(httpResponseCode);

    // Intentar reintentar hasta 5 veces
    for (int i = 0; i < 5; i++) {
      delay(2000);
      Serial.print("Reintentando anuncio (");
      Serial.print(i + 1);
      Serial.println(")...");
      sendAnnouncement();
      if (announced) break;
    }
  }
  
  http.end();
}

