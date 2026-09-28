# 📡 Bab 3 — Wi-Fi & Komunikasi MQTT

Welcome to **Bab 3!**

Di dua bab sebelumnya, ESP32 masih bekerja secara lokal.

Sekarang waktunya ESP32 mulai:

> **Terhubung ke Internet dan mengirim data ke sistem lain.**

Pada bab ini kita akan mengenal Wi-Fi, MQTT, EMQX Cloud, MQTTX, JSON, dan pengiriman data DHT11 secara real-time.

---

# 🎯 Tujuan Pembelajaran

Setelah menyelesaikan bab ini, praktikan diharapkan dapat:

- Menghubungkan ESP32 ke Wi-Fi
- Memahami MQTT
- Memahami Publisher dan Subscriber
- Membuat broker menggunakan EMQX Cloud
- Menggunakan MQTTX
- Mengirim data sensor melalui MQTT
- Mengemas data ke format JSON

---

# 📶 Wi-Fi pada ESP32

ESP32 digunakan sebagai:

```text
Station Mode / STA