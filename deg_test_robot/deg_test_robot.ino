#include <Wire.h>
#include <Servo.h>
#include <IcsHardSerialClass.h>

Servo flip_servo;

#define SERVO_GRIP_DEG 44
#define SERVO_RELEASE_DEG 145

#define BLACK_MAGNET 5
#define WHITE_MAGNET 6

#define PUL_PIN 13
#define DIR_PIN 12
#define ENA_PIN 11
#define STEPPER_DELAY 300

#define EN_PIN 2

#define ARM_HAND 2
#define ARM_MID 1
#define ARM_ROOT 0

#define ARM_HAND_ZERO 7900
#define ARM_MID_ZERO 8600
#define ARM_ROOT_ZERO 7700

#define ARM_HAND_TEST_1 (ARM_HAND_ZERO + 1000)
#define ARM_MID_TEST_1 (ARM_MID_ZERO - 4000)
#define ARM_ROOT_TEST_1 (ARM_ROOT_ZERO + 500)

#define ARM_HAND_TEST_2 (ARM_HAND_ZERO + 1300)
#define ARM_MID_TEST_2 (ARM_MID_ZERO - 3100)
#define ARM_ROOT_TEST_2 (ARM_ROOT_ZERO + 1000)

#define DEG_90 2600

IcsHardSerialClass krs(&Serial, EN_PIN, 115200, 1000);

struct Arm_pos {
  uint32_t hand;
  uint32_t mid;
  uint32_t root;
};

void setup() {
  Wire.begin();
  Wire.setClock(100000);
  flip_servo.attach(9);
  pinMode(BLACK_MAGNET, OUTPUT);
  pinMode(WHITE_MAGNET, OUTPUT);
  pinMode(PUL_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);
  krs.begin();
  flip_servo.write(SERVO_RELEASE_DEG);

  //Arm_pos arm_pos = {ARM_HAND_TEST_2, ARM_MID_TEST_2, ARM_ROOT_TEST_2};
  //move_arm(arm_pos, 50, 10);
  //for(;;);
}

void stepper_enable() {
  digitalWrite(ENA_PIN, HIGH);
}

void stepper_disable() {
  digitalWrite(ENA_PIN, LOW);
}

void stepper_move(bool dir, int steps) {
  digitalWrite(DIR_PIN, dir);
  for (int i = 0; i < steps; ++i) {
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(STEPPER_DELAY);
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(STEPPER_DELAY);
  }
}

void hold_black() {
  digitalWrite(BLACK_MAGNET, HIGH);
  digitalWrite(WHITE_MAGNET, LOW);
}

void hold_white() {
  digitalWrite(BLACK_MAGNET, LOW);
  digitalWrite(WHITE_MAGNET, HIGH);
}

void release_both() {
  digitalWrite(BLACK_MAGNET, LOW);
  digitalWrite(WHITE_MAGNET, LOW);
}

void move_arm(uint32_t deg_hand, uint32_t deg_mid, uint32_t deg_root, uint32_t num, int sleep) {
  deg_hand = max(3500, min(11500, deg_hand));
  deg_mid = max(3500, min(11500, deg_mid));
  deg_root = max(3500, min(11500, deg_root));
  uint32_t hand_now = krs.getPos(ARM_HAND);
  uint32_t mid_now = krs.getPos(ARM_MID);
  uint32_t root_now = krs.getPos(ARM_ROOT);
  for (int i = 1; i <= num; ++i) {
    krs.setPos(ARM_HAND, hand_now * (num - i) / num + deg_hand * i / num);
    krs.setPos(ARM_MID, mid_now * (num - i) / num + deg_mid * i / num);
    krs.setPos(ARM_ROOT, root_now * (num - i) / num + deg_root * i / num);
    delay(sleep);
  }
}

void move_arm(Arm_pos arm_pos, uint32_t num, int sleep) {
  move_arm(arm_pos.hand, arm_pos.mid, arm_pos.root, num, sleep);
}

void arm_zero() {
  move_arm(ARM_HAND_ZERO, ARM_MID_ZERO, ARM_ROOT_ZERO, 100, 10);
}


void send_deg(int deg) {
  int digits[5];
  for (int i = 0; i < 5; ++i){
    digits[i] = deg % 10;
    deg /= 10;
  }
  Wire.beginTransmission(8);
  for (int i = 4; i >= 0; --i){
    Wire.write('0' + digits[i]);
  }
  Wire.write(' ');
  Wire.endTransmission();
}

void loop() {
  int deg;
  deg = krs.setFree(2);
  send_deg(deg);
  deg = krs.setFree(1);
  send_deg(deg);
  deg = krs.setFree(0);
  send_deg(deg);
  Wire.beginTransmission(8);
  Wire.write('\n');
  Wire.endTransmission();
}
