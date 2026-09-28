void setup() {
    pinMode(2, OUTPUT);   // Pin 2 sebagai output (LED)
    pinMode(15, INPUT);   // Pin 15 sebagai input (tombol)
  }
  
  void loop() {
    int status = digitalRead(15);   // Baca tombol
  
    if (status == HIGH) {
      digitalWrite(2, HIGH);        // Nyalakan LED
    } else {
      digitalWrite(2, LOW);         // Matikan LED
    }
  }