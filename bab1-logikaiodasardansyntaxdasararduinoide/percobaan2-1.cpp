#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11
#define LEDPIN 2

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(LEDPIN, OUTPUT);
}

void loop() {
  delay(2000);

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("[ERROR] Gagal membaca sensor DHT11.");
    return;
  }

  Serial.print("Suhu: ");
  Serial.print(t);
  Serial.print(" C | Kelembapan: ");
  Serial.print(h);
  Serial.println(" %");

  if (t > 30.0) {
    digitalWrite(LEDPIN, HIGH);
  } else {
    digitalWrite(LEDPIN, LOW);
  }
}