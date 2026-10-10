#include <WiFi.h>

const char* ssid = "SSID_WIFI";
const char* password = "PASSWORD_WIFI";

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan ke Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi Terhubung. IP: " + WiFi.localIP().toString());
}

void loop() {
 Serial.print("Menghubungkan ke Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

    Serial.println("\nWi-Fi Terhubung. IP: " + WiFi.localIP().toString());
}
