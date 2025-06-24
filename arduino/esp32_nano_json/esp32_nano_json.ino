#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Configuración WiFi
const char* ssid = "pinv0124";           // Nombre de tu red WiFi
const char* password = "pinv0124";       // Contraseña de tu red

// IP estática
IPAddress local_IP(192, 168, 1, 103);    // IP del ESP32 WiFi
IPAddress subnet(255, 255, 255, 0);
IPAddress gateway(192, 168, 1, 1);       // Gateway típico del router

// Configuración del servidor Flask
const char* serverURL = "http://192.168.1.100:5000/api/devices/register";
const char* deviceID = "esp32-nano-01";         // Cambialo si querés otro ID
const char* hardware = "NANO-ESP32";

bool announced = false;

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("Iniciando WiFi...");

  // Configura IP estática
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Error en configuración IP estática");
  }

  WiFi.begin(ssid, password);

  // Espera conexión WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n¡Conectado por WiFi!");
  Serial.print("IP asignada: ");
  Serial.println(WiFi.localIP());

  // Enviar anuncio al servidor Flask
  sendAnnouncement();
}

void loop() {
  // Nada que hacer en el loop
}

void sendAnnouncement() {
  if (announced) return;

  HTTPClient http;

  // Crear JSON con la información del dispositivo
  DynamicJsonDocument doc(256);
  doc["id"] = deviceID;
  doc["ip"] = WiFi.localIP().toString();
  doc["hardware"] = hardware;

  String jsonString;
  serializeJson(doc, jsonString);

  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(jsonString);

  if (httpResponseCode > 0) {
    Serial.print("Anuncio enviado. Código HTTP: ");
    Serial.println(httpResponseCode);
    announced = true;
  } else {
    Serial.print("Error al enviar anuncio: ");
    Serial.println(httpResponseCode);

    // Reintento si falla
    for (int i = 0; i < 5; i++) {
      delay(2000);
      Serial.print("Reintentando (");
      Serial.print(i + 1);
      Serial.println(")...");
      sendAnnouncement();
      if (announced) break;
    }
  }

  http.end();
}

