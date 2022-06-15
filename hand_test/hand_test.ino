#include <Servo.h>

Servo flip_servo;

#define SERVO_GRIP_DEG 47
#define SERVO_RELEASE_DEG 145

#define BLACK_MAGNET 5
#define WHITE_MAGNET 6

void setup() {
  flip_servo.attach(9);
  pinMode(BLACK_MAGNET, OUTPUT);
  pinMode(WHITE_MAGNET, OUTPUT);
}

void hold_black(){
  digitalWrite(BLACK_MAGNET, HIGH);
  digitalWrite(WHITE_MAGNET, LOW);
}

void hold_white(){
  digitalWrite(BLACK_MAGNET, LOW);
  digitalWrite(WHITE_MAGNET, HIGH);
}

void release_both(){
  digitalWrite(BLACK_MAGNET, LOW);
  digitalWrite(WHITE_MAGNET, LOW);
}

void loop() {
  flip_servo.write(SERVO_RELEASE_DEG);
  hold_black();
  delay(2000);
  flip_servo.write(SERVO_GRIP_DEG);
  delay(500);
  hold_white();
  delay(500);
  flip_servo.write(SERVO_RELEASE_DEG);
  delay(2000);
  release_both();
}
