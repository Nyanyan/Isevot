#include <Servo.h>

Servo flip_servo;

void setup() {
  flip_servo.attach(9);
}

void loop() {
  flip_servo.write(47);
  delay(1000);
  flip_servo.write(145);
  delay(1000);
}
