#include <ESP32Servo.h>

#define SERVOPIN 18

Servo myServo;

void setup() {
  myServo.attach(SERVOPIN);  // Sambungkan servo ke pin GPIO18
}

void loop() {
  myServo.write(0);          // Putar ke sudut 0 derajat
  delay(1000);

  myServo.write(90);         // Putar ke sudut 90 derajat (tengah)
  delay(1000);

  myServo.write(180);        // Putar ke sudut 180 derajat
  delay(1000);
}