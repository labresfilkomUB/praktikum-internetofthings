#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#define LEDPIN 2
#define BUZZERPIN 5

const char* ssid = "HOTSPOT_PRIBADI";
const char* password = "PASSWORD_WIFI";

const char* mqtt_server = "ALAMAT_BROKER_EMQX_ANDA";
const int mqtt_port = 8883;
const char* mqtt_username = "USERNAME_MQTT";
const char* mqtt_password = "PASSWORD_MQTT";

const char* topic_telemetry = "res/iot/telemetry";
const char* topic_led = "res/iot/control/led";
const char* topic_buzzer = "res/iot/control/buzzer";

WiFiClientSecure espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String pesan = "";

  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  pesan.trim();

  Serial.println(
    "[SUBSCRIBE] Topic: " +
    String(topic) +
    " | Pesan: " +
    pesan
  );

  if (String(topic) == topic_led) {
    digitalWrite(
      LEDPIN,
      (pesan == "ON") ? HIGH : LOW
    );
  }

  if (String(topic) == topic_buzzer) {
    digitalWrite(
      BUZZERPIN,
      (pesan == "true") ? HIGH : LOW
    );
  }
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT broker...");

    if (
      client.connect(
        "ESP32-RES-Client",
        mqtt_username,
        mqtt_password
      )
    ) {
      Serial.println("Terhubung.");

      client.subscribe(topic_led);
      client.subscribe(topic_buzzer);

      Serial.println(
        "Subscribed ke: " +
        String(topic_led) +
        " & " +
        String(topic_buzzer)
      );

    } else {
      Serial.print("Gagal, rc=");
      Serial.print(client.state());
      Serial.println(". Coba lagi dalam 5 detik.");

      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(LEDPIN, OUTPUT);
  pinMode(BUZZERPIN, OUTPUT);

  digitalWrite(LEDPIN, LOW);
  digitalWrite(BUZZERPIN, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan ke Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(
    "\nWi-Fi Terhubung. IP: " +
    WiFi.localIP().toString()
  );

  espClient.setInsecure();

  client.setServer(
    mqtt_server,
    mqtt_port
  );

  client.setCallback(callback);
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

    serializeJson(
      doc,
      buffer
    );

    client.publish(
      topic_telemetry,
      buffer
    );

    Serial.println(
      "[PUBLISH] " +
      String(buffer)
    );

  } else {
    Serial.println(
      "Gagal membaca sensor DHT11!"
    );
  }

  delay(5000);
}