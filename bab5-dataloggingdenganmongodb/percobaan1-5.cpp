#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";

const char* mqtt_server = "HOST_DEPLOYMENT_EMQX";
const int mqtt_port = 8883;
const char* mqtt_username = "USERNAME_MQTT";
const char* mqtt_password = "PASSWORD_MQTT";

const char* topic_telemetry = "res/iot/telemetry";
const char* device_id = "ESP32-RES";

WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastPublish = 0;
unsigned long lastMqttAttempt = 0;
unsigned long lastWifiAttempt = 0;

const unsigned long publishInterval = 5000;

String clientId;

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);

  clientId = "ESP32-" + WiFi.macAddress();

  // Khusus praktikum: TLS tanpa verifikasi sertifikat server.
  espClient.setInsecure();

  client.setServer(mqtt_server, mqtt_port);
  client.setBufferSize(512);
}

void loop() {
  unsigned long now = millis();

  if (WiFi.status() != WL_CONNECTED) {
    if (now - lastWifiAttempt >= 10000) {
      lastWifiAttempt = now;
      WiFi.reconnect();

      Serial.println("[WIFI] Menghubungkan ulang...");
    }

    delay(10);
    return;
  }

  if (!client.connected()) {
    if (now - lastMqttAttempt >= 5000) {
      lastMqttAttempt = now;

      if (client.connect(
            clientId.c_str(),
            mqtt_username,
            mqtt_password
          )) {
        Serial.println("[MQTT] Terhubung");
      } else {
        Serial.print("[MQTT] Gagal, state=");
        Serial.println(client.state());
      }
    }

    return;
  }

  client.loop();

  now = millis();

  if (now - lastPublish < publishInterval) {
    return;
  }

  lastPublish = now;

  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();

  if (isnan(suhu) || isnan(kelembapan)) {
    Serial.println("[DHT] Pembacaan gagal");
    return;
  }

  JsonDocument doc;

  doc["suhu"] = suhu;
  doc["kelembapan"] = kelembapan;
  doc["device"] = device_id;

  char payload[256];
  serializeJson(doc, payload, sizeof(payload));

  if (client.publish(topic_telemetry, payload, false)) {
    Serial.print("[PUBLISH] ");
    Serial.println(payload);
  } else {
    Serial.println("[PUBLISH] Gagal");
  }
}