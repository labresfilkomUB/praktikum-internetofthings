# 🔀 Bab 4 — Orkestrasi Data dengan Node-RED

Welcome to **Bab 4 Praktikum Internet of Things!**

Pada Bab 3, ESP32 sudah berhasil:

- Terhubung ke Wi-Fi
- Mengirim data sensor DHT11
- Menggunakan MQTT
- Mengirim payload JSON ke EMQX

Sekarang kita akan membawa sistem tersebut satu langkah lebih jauh.

ESP32 tidak hanya akan **mengirim data**, tetapi juga dapat **menerima perintah** dari Node-RED.

```text
Bab 3
ESP32 → MQTT → MQTTX

Bab 4
ESP32 ↔ MQTT ↔ Node-RED
```

Pada bab ini kita akan belajar melakukan **orkestrasi data**, yaitu mengatur bagaimana data diterima, diproses, divisualisasikan, dan dikirim kembali ke perangkat IoT.

---

# 🎯 Tujuan Pembelajaran

Setelah menyelesaikan Bab 4, praktikan diharapkan dapat:

- Menjalankan Node-RED
- Memahami konsep `node`, `wire`, dan `flow`
- Menerima data MQTT menggunakan `mqtt in`
- Mengolah payload JSON
- Menggunakan `msg.payload`
- Membuat logika dengan node `function`
- Mengirim perintah menggunakan `mqtt out`
- Membuat ESP32 menjadi MQTT Publisher sekaligus Subscriber
- Mengontrol LED melalui Node-RED
- Membuat dashboard monitoring suhu
- Mengontrol buzzer melalui FlowFuse Dashboard

---

# 🔗 Bab 4 Melanjutkan Bab 3

Sebelum memulai praktikum, pastikan konfigurasi Bab 3 masih tersedia.

Yang akan digunakan kembali:

```text
Firmware ESP32
Deployment EMQX
MQTT Broker
Username MQTT
Password MQTT
Topic MQTT
```

Jangan membuat deployment EMQX baru jika deployment dari Bab 3 masih dapat digunakan.

---

# 🧰 Peralatan Praktikum

## Hardware

| Perangkat | Fungsi |
|---|---|
| ESP32 DevKit V1 | Mikrokontroler utama |
| DHT11 | Membaca suhu dan kelembapan |
| LED | Aktuator kontrol otomatis |
| Resistor 330 Ω | Pembatas arus LED |
| Buzzer | Aktuator yang dikontrol melalui dashboard |
| Breadboard | Media rangkaian |
| Kabel Jumper | Penghubung komponen |

## Software

| Software | Fungsi |
|---|---|
| Arduino IDE | Pemrograman ESP32 |
| Node.js | Runtime yang dibutuhkan Node-RED |
| Node-RED | Orkestrasi data IoT |
| FlowFuse Dashboard | Monitoring dan kontrol berbasis web |
| EMQX Cloud | MQTT Broker |

---

# 🟩 Install Node.js & Node-RED

Sebelum menggunakan Node-RED, install **Node.js versi LTS**.

Setelah instalasi selesai, buka:

```text
Command Prompt
PowerShell
Terminal
```

Cek instalasi Node.js:

```bash
node --version
```

Cek npm:

```bash
npm --version
```

Jika keduanya berhasil menampilkan nomor versi, install Node-RED:

```bash
npm install -g node-red
```

Jalankan Node-RED:

```bash
node-red
```

Kemudian buka:

```text
http://localhost:1880
```

---

# 🧩 Konsep Dasar Node-RED

Node-RED menggunakan konsep **flow-based programming**.

Ada tiga istilah penting:

| Istilah | Fungsi |
|---|---|
| **Node** | Blok yang menjalankan suatu fungsi |
| **Wire** | Penghubung antar-node |
| **Flow** | Kumpulan node dan wire yang membentuk alur proses |

Contoh:

```text
mqtt in
   ↓
 json
   ↓
function
   ↓
mqtt out
```

Data masuk dari MQTT, diubah menjadi object, diproses, lalu dikirim kembali.

---

# 📨 Message dan `msg.payload`

Data yang berpindah antar-node disimpan dalam sebuah object bernama:

```javascript
msg
```

Data utama biasanya berada pada:

```javascript
msg.payload
```

Pada Bab 3, ESP32 mengirim data seperti:

```json
{
  "suhu": 29.5,
  "kelembapan": 65.0,
  "device": "ESP32-RES"
}
```

Setelah melewati node `json`, setiap data dapat diakses melalui:

| Syntax | Isi |
|---|---|
| `msg.payload.suhu` | Nilai suhu |
| `msg.payload.kelembapan` | Nilai kelembapan |
| `msg.payload.device` | Identitas perangkat |

Contoh:

```javascript
var suhu = msg.payload.suhu;
```

---

# 📦 Node `json`

Payload MQTT dapat diterima sebagai teks JSON.

Node:

```text
json
```

digunakan untuk mengubah JSON menjadi object JavaScript.

Sebelum:

```json
{
  "suhu": 29.5,
  "kelembapan": 65.0
}
```

Setelah diproses:

```javascript
msg.payload.suhu
```

```javascript
msg.payload.kelembapan
```

Alurnya:

```text
MQTT
 ↓
mqtt in
 ↓
JSON String
 ↓
json
 ↓
JavaScript Object
```

---

# 📡 MQTT pada Node-RED

Ada dua node MQTT utama yang digunakan.

| Node | Fungsi |
|---|---|
| `mqtt in` | Subscribe pesan dari MQTT Broker |
| `mqtt out` | Publish pesan menuju MQTT Broker |

## `mqtt in`

Digunakan untuk menerima data dari ESP32.

```text
ESP32
  ↓
EMQX
  ↓
mqtt in
  ↓
Node-RED
```

## `mqtt out`

Digunakan untuk mengirim perintah dari Node-RED.

```text
Node-RED
   ↓
mqtt out
   ↓
EMQX
   ↓
ESP32
```

---

# 📨 Topic MQTT Bab 4

Bab 4 menggunakan tiga topic utama.

| Topic | Arah | Fungsi |
|---|---|---|
| `res/iot/telemetry` | ESP32 → Node-RED | Data sensor |
| `res/iot/control/led` | Node-RED → ESP32 | Kontrol LED |
| `res/iot/control/buzzer` | Node-RED → ESP32 | Kontrol buzzer |

Secara sederhana:

```text
                 TELEMETRY

ESP32 ──────────────────────► Node-RED

       res/iot/telemetry
```

Sedangkan untuk kontrol:

```text
                  CONTROL

ESP32 ◄────────────────────── Node-RED

       res/iot/control/led
       res/iot/control/buzzer
```

---

# 🧠 Node `function`

Node `function` digunakan untuk menjalankan kode JavaScript.

Pada Percobaan 1, suhu digunakan untuk menentukan kondisi LED.

```javascript
var suhu = msg.payload.suhu;

if (suhu > 30) {
    msg.payload = "ON";
} else {
    msg.payload = "OFF";
}

return msg;
```

Logikanya:

```text
Suhu > 30°C
     ↓
    ON
     ↓
LED Menyala
```

Sedangkan:

```text
Suhu ≤ 30°C
     ↓
    OFF
     ↓
 LED Mati
```

---

# 💻 Syntax Node-RED yang Perlu Dipahami

| Syntax | Fungsi |
|---|---|
| `msg.payload` | Mengakses isi pesan |
| `msg.payload.suhu` | Mengakses nilai suhu |
| `msg.payload.kelembapan` | Mengakses nilai kelembapan |
| `msg.payload = "ON"` | Mengubah payload menjadi `ON` |
| `msg.payload = "OFF"` | Mengubah payload menjadi `OFF` |
| `return msg` | Mengirim message ke node berikutnya |
| `if / else` | Membuat keputusan berdasarkan kondisi |

Contoh:

```javascript
if (msg.payload.suhu > 30) {
    msg.payload = "ON";
} else {
    msg.payload = "OFF";
}

return msg;
```

---

# ↩️ ESP32 sebagai MQTT Subscriber

Pada Bab 3, ESP32 terutama digunakan sebagai:

```text
Publisher
```

Pada Bab 4, ESP32 menjadi:

```text
Publisher + Subscriber
```

ESP32 tetap melakukan:

```cpp
client.publish();
```

dan sekarang juga menggunakan:

```cpp
client.subscribe();
```

---

# 💻 Syntax MQTT ESP32

| Syntax | Fungsi |
|---|---|
| `client.setServer()` | Menentukan MQTT Broker |
| `client.setCallback()` | Menentukan fungsi callback |
| `client.subscribe()` | Subscribe ke topic |
| `client.publish()` | Publish data |
| `client.connected()` | Mengecek koneksi MQTT |
| `client.loop()` | Menjaga proses MQTT berjalan |

Subscribe topic:

```cpp
client.subscribe(topic_led);
client.subscribe(topic_buzzer);
```

Publish telemetri:

```cpp
client.publish(topic_telemetry, buffer);
```

---

# 🔄 Fungsi `callback()`

Ketika ESP32 menerima pesan MQTT, PubSubClient menjalankan:

```cpp
callback()
```

Program yang digunakan:

```cpp
void callback(char* topic, byte* payload, unsigned int length) {
  String pesan = "";

  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  pesan.trim();

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
```

Alurnya:

```text
MQTT Message
     ↓
 callback()
     ↓
 Cek Topic
     ↓
 Cek Payload
     ↓
LED / Buzzer
```

---

# 💡 Percobaan 1 — Kontrol LED Otomatis

Percobaan pertama menggunakan data suhu DHT11 untuk mengontrol LED.

Alurnya:

```text
DHT11
  ↓
ESP32
  ↓
JSON
  ↓
MQTT
  ↓
EMQX
  ↓
mqtt in
  ↓
json
  ↓
function
  ↓
ON / OFF
  ↓
mqtt out
  ↓
EMQX
  ↓
ESP32
  ↓
LED
```

Flow Node-RED:

```text
mqtt in
   ↓
 json
   ↓
function
   ├─────────→ debug
   │
   ↓
mqtt out
```

Topic input:

```text
res/iot/telemetry
```

Topic output:

```text
res/iot/control/led
```

---

# 🐞 Node `debug`

Node `debug` digunakan untuk melihat data pada sidebar Debug.

Pada Percobaan 1, output yang diharapkan:

```text
ON
```

atau:

```text
OFF
```

Jika `debug` menampilkan nilai yang benar tetapi LED tidak berubah, periksa:

```text
Topic mqtt out
Koneksi MQTT ESP32
Subscription ESP32
Serial Monitor
```

---

# 📊 FlowFuse Dashboard

Bab 4 menggunakan:

```text
@flowfuse/node-red-dashboard
```

Install melalui:

```text
Node-RED
   ↓
Menu
   ↓
Manage Palette
   ↓
Install
   ↓
@flowfuse/node-red-dashboard
```

Gunakan **FlowFuse Dashboard**, bukan package:

```text
node-red-dashboard
```

versi lama.

---

# 🧱 Struktur FlowFuse Dashboard

Dashboard menggunakan struktur:

```text
Base
 ↓
Page
 ↓
Group
 ↓
Widget
```

Konfigurasi praktikum:

```text
Base Path  : /dashboard
Page Name  : Monitoring IoT
Group Name : Monitoring IoT
```

Dashboard dapat dibuka melalui:

```text
http://localhost:1880/dashboard
```

---

# 📈 Widget Dashboard

Bab 4 menggunakan tiga widget utama.

| Widget | Fungsi |
|---|---|
| `ui-switch` | Kontrol buzzer |
| `ui-gauge` | Menampilkan suhu saat ini |
| `ui-chart` | Menampilkan perubahan suhu |

---

# 🔔 Percobaan 2 — Kontrol Buzzer

Buzzer dikontrol secara manual melalui dashboard.

Alurnya:

```text
ui-switch
    ↓
mqtt out
    ↓
EMQX
    ↓
ESP32
    ↓
callback()
    ↓
Buzzer
```

Topic:

```text
res/iot/control/buzzer
```

Payload ketika switch aktif:

```text
true
```

Payload ketika switch tidak aktif:

```text
false
```

---

# 🌡️ Gauge Suhu

Node:

```text
ui-gauge
```

digunakan untuk menampilkan suhu saat ini.

Value:

```javascript
msg.payload.suhu
```

Alur:

```text
mqtt in
   ↓
 json
   ↓
ui-gauge
```

---

# 📉 Chart Suhu

Node:

```text
ui-chart
```

digunakan untuk menampilkan perubahan suhu terhadap waktu.

Data juga harus berasal langsung dari:

```text
json
```

Alur:

```text
mqtt in
   ↓
 json
   ↓
ui-chart
```

---

# ⚠️ Jangan Salah Menghubungkan Flow!

Perhatikan alur berikut:

```text
                    ┌────→ function → mqtt out
                    │
mqtt in → json ─────┼────→ ui-gauge
                    │
                    └────→ ui-chart
```

Gauge dan chart harus mengambil data **langsung dari output `json`**.

Jangan:

```text
json → function → gauge
```

karena setelah melewati `function`, payload sudah berubah menjadi:

```text
ON
```

atau:

```text
OFF
```

dan tidak lagi berisi:

```javascript
msg.payload.suhu
```

---

# 🌐 The Big Picture

Setelah seluruh sistem selesai:

```text
                    DHT11
                      ↓
                    ESP32
                      ↓
                 Telemetry
                      ↓
                    EMQX
                      ↓
                   mqtt in
                      ↓
                     json
                 ┌────┼────┐
                 │    │    │
                 │    │    └────→ Chart
                 │    │
                 │    └─────────→ Gauge
                 │
                 ↓
              function
                 ↓
              ON / OFF
                 ↓
              mqtt out
                 ↓
                EMQX
                 ↓
                ESP32
                 ↓
                 LED
```

Sedangkan kontrol buzzer:

```text
Dashboard
    ↓
ui-switch
    ↓
mqtt out
    ↓
EMQX
    ↓
ESP32
    ↓
Buzzer
```

---

# 🔍 Troubleshooting

## MQTT In Selalu `connecting`

Periksa:

```text
Broker
Port 8883
Username
Password
TLS
Koneksi Internet
```

---

## Debug Tidak Menampilkan Data

Periksa:

```text
mqtt in
 ↓
json
 ↓
function
```

Pastikan seluruh node sudah dihubungkan menggunakan wire dan sudah:

```text
Deploy
```

---

## LED Tidak Merespons

Jika `debug` sudah menunjukkan:

```text
ON
```

atau:

```text
OFF
```

tetapi LED tidak berubah, periksa:

```text
Topic mqtt out
Topic subscribe ESP32
Koneksi MQTT
GPIO LED
Serial Monitor [SUBSCRIBE]
```

---

## Gauge Tidak Menampilkan Suhu

Pastikan sumber wire:

```text
json → ui-gauge
```

dan value:

```javascript
msg.payload.suhu
```

---

## Buzzer Tidak Merespons

Pastikan payload dari `ui-switch` adalah:

```text
true
```

dan:

```text
false
```

bukan:

```text
ON
OFF
```

---

# 💾 Simpan Flow untuk Bab 5!

Bab 4 belum menjadi akhir dari perjalanan flow Node-RED kita.

Flow ini akan digunakan kembali pada **Bab 5**.

Setelah praktikum selesai:

1. Pastikan seluruh flow bekerja.
2. Klik **Deploy**.
3. Export flow:

```text
☰
↓
Export
↓
Current Flow
```

4. Simpan sebagai JSON.

Contoh:

```text
flow-bab4-final.json
```

Jangan hapus:

```text
mqtt in
json
Konfigurasi Broker
```

Pada Bab 5, flow ini akan dikembangkan lagi untuk melakukan:

```text
DATA LOGGING
     ↓
   MongoDB
```

---

# 🧠 Hal yang Harus Dipahami

Sebelum meninggalkan Bab 4, pastikan kalian dapat menjawab:

- Apa perbedaan `mqtt in` dan `mqtt out`?
- Apa fungsi node `json`?
- Apa itu `msg.payload`?
- Mengapa `function` mengubah payload menjadi `ON/OFF`?
- Mengapa Gauge dan Chart harus mengambil data dari node `json`?
- Bagaimana ESP32 dapat menjadi Publisher sekaligus Subscriber?
- Apa fungsi `callback()`?
- Mengapa LED dan buzzer menggunakan topic MQTT yang berbeda?
- Apa perbedaan telemetri dan kontrol?
- Bagaimana data bergerak dari DHT11 hingga dashboard?

---

# 🚀 Pesan dari Asprak

Di Bab 3 kita membuat perangkat yang bisa **mengirim data ke Internet**.

Di Bab 4, perangkat tersebut mulai bisa:

```text
MENGIRIM
+
MENERIMA
+
MEMPROSES
+
MENGONTROL
+
MEMVISUALISASIKAN
```

Di sinilah sistem kita mulai terasa seperti sistem IoT yang utuh.

Jangan hanya menghafalkan posisi node.

Coba pahami:

> **Data datang dari mana, berubah menjadi apa, melewati node apa, dan akhirnya pergi ke mana?**

Kalau kalian sudah bisa membaca sebuah flow seperti membaca alur cerita, Node-RED akan jauh lebih mudah dipahami.

Dan seperti biasa:

> **Kalau belum jalan, Debug dulu. Jangan langsung bongkar semuanya. :D**

Semangat Praktikum Bab 4! 🔧📡

**— Asisten Praktikum Internet of Things 2026**  
**Laboratorium Robotika dan Embedded System FILKOM UB**