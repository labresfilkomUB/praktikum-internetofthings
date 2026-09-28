#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Alamat 0x27, 16 kolom, 2 baris

void setup() {
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("RES Lab - IoT");

  lcd.setCursor(0, 1);
  lcd.print("Praktikum Bab 2");
}

void loop() {
  // Konten statis; nilai dinamis dapat dikembangkan
  // pada percobaan lanjutan
}