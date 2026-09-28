#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11

const char* ssid = "HOTSPOT_PRIBADI";
const char* password = "PASSWORD_WIFI";
const char* mqtt_server = "m66b7a61.ala.asia-southeast1.emqxsl.com";
const int mqtt_port = 8883;
const char* mqtt_username = "reslab";
const char* mqtt_password = "12345";
const char* mqtt_topic = "res/iot/telemetry";

WiFiClientSecure espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Terhubung. IP: " + WiFi.localIP().toString());

  espClient.setInsecure(); // Lewati validasi sertifikat CA
  client.setServer(mqtt_server, mqtt_port);
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT broker...");
    if (client.connect("ESP32-RES-Client", mqtt_username, mqtt_password)) {
      Serial.println("Terhubung.");
    } else {
      Serial.print("Gagal, rc=");
      Serial.print(client.state());
      Serial.println(". Coba lagi dalam 5 detik.");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (!isnan(t) && !isnan(h)) {
    StaticJsonDocument<200> doc;
    doc["suhu"] = t;
    doc["kelembapan"] = h;
    doc["device"] = "ESP32-RES";

    char buffer[256];
    serializeJson(doc, buffer);

    client.publish(mqtt_topic, buffer);
    Serial.println("[PUBLISH] " + String(buffer));
  }

  delay(5000);
}