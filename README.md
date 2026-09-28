# 🌐 Praktikum Internet of Things 2026
### Laboratorium Robotika dan Embedded System — FILKOM Universitas Brawijaya

Selamat datang di repositori **Praktikum Internet of Things**!

Praktikum ini dirancang untuk membawa kalian dari hal yang paling dasar seperti membaca input dan mengendalikan output pada ESP32, hingga membangun alur data IoT yang lebih lengkap menggunakan MQTT, Node-RED, dan database.

> **Pesan dari Asisten Praktikum IoT**
>
> Jangan terlalu fokus untuk menghafalkan semua syntax.
> Yang paling penting adalah memahami **alur kerja sistem**, mengetahui apa yang menjadi **input**, bagaimana data **diproses**, dan apa yang menjadi **output**.
>
> Error adalah bagian dari praktikum. Kalau program belum berjalan, jangan langsung panik — cek wiring, baca Serial Monitor, periksa library, lalu debugging pelan-pelan.
>
> **Semangat praktikum! Satu error hari ini, satu skill baru besok. 🚀**

---

# 📖 Tentang Course Ini

Pada praktikum ini kita akan menggunakan **ESP32 DevKit V1** sebagai perangkat utama.

ESP32 akan digunakan untuk:

- Membaca sensor
- Mengendalikan aktuator
- Mengolah input digital dan analog
- Terhubung ke Wi-Fi
- Mengirim data melalui MQTT
- Mengintegrasikan perangkat dengan Node-RED
- Menyimpan data IoT pada database

Secara sederhana, perjalanan kita akan bergerak dari:

```text
Sensor / Input
      ↓
    ESP32
      ↓
Pemrosesan Data
      ↓
Wi-Fi & MQTT
      ↓
   Node-RED
      ↓
   Database