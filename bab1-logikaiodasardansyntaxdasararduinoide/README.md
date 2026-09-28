
---

## 2. README Bab 1 — Logika I/O Dasar & Syntax Arduino IDE

```markdown
# 🔌 Bab 1 — Logika I/O Dasar & Syntax Dasar Arduino IDE

Selamat datang di Bab 1 Praktikum Internet of Things!

Bab ini menjadi fondasi sebelum kita masuk ke komunikasi, jaringan, dan pengolahan data IoT.

Pada bab ini kita akan belajar bagaimana **ESP32 membaca kondisi dari dunia luar dan memberikan respons melalui output**.

---

# 🎯 Tujuan Pembelajaran

Setelah menyelesaikan Bab 1, praktikan diharapkan dapat:

- Menggunakan pin digital sebagai input dan output
- Menggunakan `digitalRead()` dan `digitalWrite()`
- Menggunakan komunikasi Serial
- Membaca suhu dan kelembapan menggunakan DHT11
- Menggunakan logika `if/else`
- Mengontrol LED berdasarkan suatu kondisi

---

# 🧰 Komponen Utama

| Komponen | Fungsi |
|---|---|
| ESP32 DevKit V1 | Mikrokontroler utama |
| Push Button | Input digital |
| LED | Output digital |
| Resistor 330 Ω | Membatasi arus LED |
| Resistor 10 kΩ | Pull-down Push Button |
| DHT11 | Sensor suhu dan kelembapan |
| Breadboard | Media perakitan rangkaian |

---

# 💻 Syntax Dasar

## Digital I/O

| Syntax | Fungsi |
|---|---|
| `pinMode(pin, INPUT)` | Menjadikan GPIO sebagai input |
| `pinMode(pin, OUTPUT)` | Menjadikan GPIO sebagai output |
| `digitalRead(pin)` | Membaca kondisi `HIGH` atau `LOW` |
| `digitalWrite(pin, HIGH)` | Memberikan logika HIGH |
| `digitalWrite(pin, LOW)` | Memberikan logika LOW |

Contoh:

```cpp
pinMode(2, OUTPUT);
pinMode(15, INPUT);

int status = digitalRead(15);

if (status == HIGH) {
  digitalWrite(2, HIGH);
}