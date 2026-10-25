# 🗄️ Bab 5 — Data Logging dengan MongoDB

Pada bab sebelumnya, data sensor sudah dapat dikirim melalui MQTT dan diolah menggunakan Node-RED. Pada bab ini, alur tersebut dikembangkan untuk **menyimpan riwayat pengukuran ke MongoDB**.

Firmware Percobaan 1 membaca suhu dan kelembapan dari DHT11, lalu mengirimkannya sebagai JSON ke broker EMQX melalui MQTT over TLS.

> **Batas cakupan kode:** kode ESP32 yang menjadi acuan hanya menangani pembacaan sensor dan pengiriman MQTT. Penyimpanan ke MongoDB dilakukan oleh integrasi Node-RED yang perlu dikonfigurasi secara terpisah, bukan langsung oleh ESP32.

---

## 🎯 Tujuan Pembelajaran

Setelah menyelesaikan praktikum, praktikan diharapkan dapat:

- Membaca suhu dan kelembapan menggunakan ESP32 dan DHT11.
- Mengirim telemetri dalam format JSON melalui MQTT.
- Memahami penjadwalan pengiriman menggunakan `millis()`.
- Memahami mekanisme koneksi ulang Wi-Fi dan MQTT.
- Menghubungkan alur telemetri dengan Node-RED dan MongoDB.
- Membedakan data monitoring sesaat dengan riwayat data yang tersimpan.

## 🔗 Arsitektur Sistem

```text
DHT11
  ↓ suhu dan kelembapan
ESP32
  ↓ MQTT over TLS, port 8883
EMQX
  ↓ topic: res/iot/telemetry
Node-RED
  ↓ parsing, validasi, dan penambahan waktu penerimaan
MongoDB
  ↓
Riwayat data sensor
```

**Data logging** adalah pencatatan data secara berkala agar pengukuran sebelumnya dapat ditinjau kembali. Broker MQTT mendistribusikan pesan; database menyimpan riwayatnya.

## 🧰 Peralatan dan Prasyarat

### Hardware

| Perangkat | Kegunaan |
|---|---|
| ESP32 | Membaca sensor dan mengirim data |
| DHT11 | Mengukur suhu dan kelembapan |
| Breadboard dan kabel jumper | Menghubungkan rangkaian |
| Kabel USB data | Upload program dan Serial Monitor |

### Software dan Layanan

| Kebutuhan | Keterangan |
|---|---|
| Arduino IDE dan board package ESP32 | Kompilasi serta upload firmware |
| DHT sensor library by Adafruit | Menyediakan `DHT.h`; pasang dependensi yang diminta Library Manager |
| PubSubClient by Nick O'Leary | Koneksi dan publish MQTT |
| ArduinoJson **versi 7** | Sesuai penggunaan `JsonDocument doc;` pada kode |
| Wi-Fi 2,4 GHz | Koneksi jaringan untuk board ESP32 yang digunakan |
| Deployment EMQX | Endpoint MQTT TLS, akun MQTT, dan izin publish topic |
| MQTTX atau Node-RED | Memeriksa pesan yang diterima broker |
| Node-RED dan MongoDB | Diperlukan untuk tahap penyimpanan data |

`WiFi.h` dan `WiFiClientSecure.h` tersedia melalui board package ESP32. Gunakan deployment EMQX dari bab sebelumnya jika masih tersedia.

## 🔌 Rangkaian DHT11

| Pin DHT11 | Pin ESP32 |
|---|---|
| VCC | 3V3, untuk sensor/modul yang mendukung 3,3 V |
| DATA / OUT | GPIO 4 |
| GND | GND |

Periksa label dan pinout sensor karena urutan kaki DHT11 tanpa modul dapat berbeda dari modul tiga pin. Untuk sensor tanpa modul, gunakan resistor pull-up DATA ke 3V3 sesuai datasheet, umumnya 4,7–10 kΩ. Pastikan sinyal DATA tidak melebihi level logika 3,3 V ESP32.

## ⚙️ Konfigurasi Firmware

Gunakan kode Percobaan 1 yang diberikan sebagai sketch Arduino. Jika bekerja melalui Arduino IDE, salin kode ke sketch `.ino` baru dan pilih board serta port ESP32 yang sesuai.

Sesuaikan parameter berikut sebelum upload:

| Parameter | Nilai awal | Penyesuaian |
|---|---|---|
| `ssid` | `NAMA_WIFI` | Nama jaringan Wi-Fi |
| `password` | `PASSWORD_WIFI` | Password Wi-Fi |
| `mqtt_server` | `HOST_DEPLOYMENT_EMQX` | Host broker, tanpa awalan `mqtts://` atau path |
| `mqtt_port` | `8883` | Port MQTT TLS dari deployment |
| `mqtt_username` | `USERNAME_MQTT` | Username autentikasi broker |
| `mqtt_password` | `PASSWORD_MQTT` | Password autentikasi broker |
| `topic_telemetry` | `res/iot/telemetry` | Topic pengiriman data |
| `device_id` | `ESP32-RES` | Identitas perangkat dalam payload |
| `DHTPIN` | `4` | GPIO DATA DHT11 |
| `publishInterval` | `5000` | Interval percobaan pengiriman dalam milidetik |

Untuk beberapa perangkat, gunakan `device_id` berbeda agar datanya mudah dibedakan. Identitas ini terpisah dari **MQTT client ID**, yang dibentuk dari awalan `ESP32-` dan alamat MAC Wi-Fi.

> Jangan menyertakan kredensial asli dalam commit, tangkapan layar, atau laporan publik.

## 💻 Cara Kerja Program

### 1. Inisialisasi melalui `setup()`

Program melakukan:

1. Membuka Serial pada **115200 baud**.
2. Menginisialisasi sensor DHT11.
3. Mengaktifkan mode Wi-Fi station (`WIFI_STA`) dan auto-reconnect.
4. Memulai koneksi Wi-Fi.
5. Membentuk MQTT client ID berdasarkan alamat MAC.
6. Mengatur koneksi TLS, alamat broker, dan buffer MQTT sebesar 512 byte.

### 2. Menjaga koneksi melalui `loop()`

| Kondisi | Respons program |
|---|---|
| Wi-Fi belum terhubung | Memanggil `WiFi.reconnect()` dengan interval percobaan minimal 10 detik |
| Wi-Fi terhubung, MQTT belum terhubung | Mencoba autentikasi MQTT dengan interval percobaan minimal 5 detik |
| MQTT terhubung | Memanggil `client.loop()` untuk memelihara koneksi |
| Interval publish tercapai | Membaca sensor dan mencoba mengirim JSON |

Penjadwalan menggunakan selisih `millis()`, bukan `delay(5000)`. Namun, proses koneksi jaringan masih dapat menunggu hingga berhasil atau timeout; program tidak sepenuhnya nonblocking.

### 3. Membaca dan mengirim data

Setiap interval pengiriman, program membaca:

```cpp
float suhu = dht.readTemperature();
float kelembapan = dht.readHumidity();
```

Jika salah satu hasil berupa `NaN`, data tidak dikirim. Pembacaan dicoba kembali pada interval berikutnya.

Data yang valid dikemas menggunakan ArduinoJson dan diserialisasi ke buffer `payload` sebesar 256 byte. Pesan kemudian dikirim dengan:

```cpp
client.publish(topic_telemetry, payload, false);
```

Argumen `false` berarti pesan **tidak retained**. Publish PubSubClient menggunakan **QoS 0**, sehingga tidak ada acknowledgment MQTT untuk memastikan pesan sudah diterima broker atau tersimpan di database.

## 📨 Format Telemetri

Topic:

```text
res/iot/telemetry
```

Contoh payload; angka berikut hanya ilustrasi:

```json
{
  "suhu": 29.0,
  "kelembapan": 65.0,
  "device": "ESP32-RES"
}
```

| Field | Tipe | Makna |
|---|---|---|
| `suhu` | Number | Suhu dalam derajat Celsius (°C) |
| `kelembapan` | Number | Kelembapan relatif (% RH) |
| `device` | String | Identitas perangkat |

Payload firmware **belum memiliki timestamp**. Jika waktu ditambahkan di Node-RED, waktu tersebut adalah waktu penerimaan/pemrosesan, bukan waktu pengukuran yang dicatat ESP32.

## 🧪 Percobaan 1 — Mengirim Telemetri

1. Hubungkan DHT11 sesuai tabel rangkaian.
2. Siapkan library dan konfigurasi Wi-Fi serta MQTT.
3. Verifikasi dan upload sketch ke ESP32.
4. Buka Serial Monitor pada **115200 baud**.
5. Hubungkan MQTTX atau Node-RED ke broker yang sama menggunakan TLS dan kredensial yang sesuai. Gunakan client ID berbeda dari ESP32.
6. Subscribe ke `res/iot/telemetry` dan pastikan akun subscriber memiliki izin membaca topic tersebut.
7. Amati pesan baru sekitar setiap 5 detik selama koneksi dan pembacaan sensor berhasil.

Contoh keluaran Serial Monitor:

```text
[MQTT] Terhubung
[PUBLISH] {"suhu":29,"kelembapan":65,"device":"ESP32-RES"}
```

Log `[PUBLISH]` menunjukkan pemanggilan publish berhasil menurut library, **bukan bukti data sudah tersimpan di MongoDB**. Karena pesan tidak retained, subscriber yang baru terhubung perlu menunggu pengiriman berikutnya.

## 🔀 Integrasi Lanjutan — Node-RED ke MongoDB

Tahap ini merupakan konfigurasi tambahan di luar firmware dan belum disertai file flow pada folder ini.

### 1. Siapkan penerima MQTT

Buat alur berikut di Node-RED:

```text
mqtt in → json → function → node MongoDB (insert)
                     ↓
                   debug
```

- Konfigurasikan `mqtt in` ke endpoint EMQX yang sama dengan TLS dan autentikasi yang sesuai.
- Subscribe ke `res/iot/telemetry`.
- Atur node `json` agar menghasilkan object JavaScript. Jika `mqtt in` sudah menghasilkan object, konversi ini tidak diperlukan.
- Gunakan `debug` untuk memeriksa dokumen sebelum penyimpanan.

### 2. Validasi dan tambahkan waktu penerimaan

Contoh isi node `function` berikut mengasumsikan `msg.payload` sudah berupa object:

```javascript
const data = msg.payload;

if (
    !data ||
    !Number.isFinite(data.suhu) ||
    !Number.isFinite(data.kelembapan) ||
    typeof data.device !== "string" ||
    data.device.trim() === ""
) {
    node.warn("Payload telemetri tidak valid");
    return null;
}

msg.payload = {
    suhu: data.suhu,
    kelembapan: data.kelembapan,
    device: data.device,
    receivedAt: new Date()
};

return msg;
```

### 3. Simpan ke MongoDB

Siapkan MongoDB lokal atau layanan MongoDB yang dapat dijangkau Node-RED. Gunakan node integrasi MongoDB yang kompatibel dengan versi Node-RED dan MongoDB, lalu ikuti dokumentasinya untuk memilih operasi **insert satu dokumen** dari `msg.payload`.

Contoh penamaan untuk praktikum:

| Pengaturan | Contoh |
|---|---|
| Database | `praktikum_iot` |
| Collection | `telemetry` |
| Operasi | Insert dokumen baru untuk setiap pesan |

Nama node dan cara konfigurasi berbeda antar-package; jangan mengasumsikan semua node MongoDB memiliki pengaturan yang sama. Simpan kredensial pada konfigurasi kredensial node, bukan pada payload atau flow yang dibagikan. Untuk layanan cloud, batasi akses jaringan ke sumber yang diperlukan.

### 4. Verifikasi hasil penyimpanan

Setelah flow di-deploy, periksa collection melalui MongoDB Compass atau `mongosh`. Contoh query jika menggunakan nama di atas:

```javascript
use praktikum_iot
db.telemetry.find({ device: "ESP32-RES" }).sort({ receivedAt: -1 }).limit(10)
```

Pastikan jumlah dokumen bertambah, field sesuai payload, dan `receivedAt` tersimpan sebagai BSON Date apabila node integrasi mempertahankan tipe JavaScript `Date`. MongoDB akan menambahkan `_id` jika dokumen belum memilikinya.

## 🔐 Keamanan dan Batasan

- Kode menggunakan `espClient.setInsecure()`: lalu lintas memakai TLS, tetapi **sertifikat server tidak diverifikasi**. Konfigurasi ini khusus praktikum dan rentan terhadap penyamaran server. Untuk produksi, gunakan CA yang dipercaya dan verifikasi sertifikat server.
- Firmware hanya menjadi **publisher**; tidak ada subscription, callback kontrol, LED, atau buzzer pada kode ini.
- Tidak ada antrean lokal atau pengiriman ulang data yang gagal. Saat koneksi terputus, riwayat pengukuran selama gangguan tidak direkam oleh firmware.
- Interval 5 detik adalah jadwal percobaan pengiriman, bukan jaminan data tersimpan setiap tepat 5 detik.
- Pesan MQTT non-retained bukan mekanisme penyimpanan historis. Penyimpanan perlu diverifikasi di MongoDB.

## 🛠️ Troubleshooting

| Gejala | Pemeriksaan |
|---|---|
| `[WIFI] Menghubungkan ulang...` berulang | Periksa SSID, password, jaringan 2,4 GHz, dan jangkauan sinyal |
| `[MQTT] Gagal, state=...` | Periksa host, port TLS, konektivitas, autentikasi, dan aturan akses broker; cocokkan nilai state dengan dokumentasi PubSubClient |
| `[DHT] Pembacaan gagal` | Periksa GPIO 4, tipe DHT11, catu daya, GND, urutan kaki, dan pull-up DATA |
| `[PUBLISH] Gagal` | Periksa koneksi MQTT dan kapasitas buffer terhadap panjang topic serta payload |
| `JsonDocument` gagal dikompilasi | Pastikan menggunakan ArduinoJson versi 7 sesuai kode |
| `DHT.h` atau `PubSubClient.h` tidak ditemukan | Pasang library yang sesuai melalui Library Manager |
| Serial menampilkan karakter tidak terbaca | Atur baud rate ke 115200 |
| MQTT menerima pesan, tetapi MongoDB kosong | Periksa parsing JSON, keluaran function, status koneksi database, operasi insert, serta database dan collection tujuan |

## ✅ Checklist Keberhasilan

- [ ] ESP32 berhasil terhubung ke Wi-Fi dan MQTT.
- [ ] Payload memuat `suhu`, `kelembapan`, dan `device`.
- [ ] Subscriber menerima pesan baru sekitar setiap 5 detik saat sistem normal.
- [ ] ESP32 mencoba terhubung kembali setelah koneksi terganggu.
- [ ] Untuk tahap integrasi, Node-RED menghasilkan dokumen valid dengan waktu penerimaan.
- [ ] Untuk tahap integrasi, dokumen baru benar-benar terlihat di MongoDB.

> **Inti praktikum:** ESP32 mengukur, MQTT mengirimkan pesan, Node-RED mengolah, dan MongoDB menyimpan riwayat.
