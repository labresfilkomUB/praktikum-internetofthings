#define POTPIN 34
#define LEDPIN 18

void setup() {
  Serial.begin(115200);
  pinMode(LEDPIN, OUTPUT);
}

void loop() {
  int nilaiADC = analogRead(POTPIN);
  int nilaiPWM = map(nilaiADC, 0, 4095, 0, 255);

  analogWrite(LEDPIN, nilaiPWM);

  Serial.print("Nilai ADC: ");
  Serial.print(nilaiADC);
  Serial.print(" | Nilai PWM: ");
  Serial.println(nilaiPWM);

  delay(100);
}