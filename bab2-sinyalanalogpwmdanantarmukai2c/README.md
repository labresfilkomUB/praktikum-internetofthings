
---

## 3. README Bab 2 — Sinyal Analog, PWM & I2C

```markdown
# 🎛️ Bab 2 — Sinyal Analog, PWM & Antarmuka I2C

Pada Bab 1 kita sudah mengenal input dan output digital.

Sekarang kita akan masuk ke dunia yang sedikit lebih fleksibel:

- Bagaimana ESP32 membaca nilai analog?
- Bagaimana nilai analog diubah menjadi angka?
- Bagaimana mengatur tingkat kecerahan LED?
- Bagaimana mengendalikan posisi servo?
- Bagaimana berkomunikasi dengan LCD menggunakan hanya dua jalur?

Welcome to **Bab 2!**

---

# 🎯 Tujuan Pembelajaran

Pada bab ini praktikan akan mempelajari:

- ADC pada ESP32
- Potensiometer
- PWM dan duty cycle
- Motor Servo SG90
- Komunikasi I2C
- LCD 16x2 I2C

---

# 🎚️ ADC — Analog to Digital Converter

ADC mengubah tegangan analog menjadi angka digital.

ESP32 menggunakan ADC 12-bit:

```text
0 V   → 0
3.3 V → 4095