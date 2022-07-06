#include <Wire.h>
#include <Servo.h>
#include <IcsHardSerialClass.h>

Servo flip_servo;

#define HW 8

#define SERVO_GRIP_DEG 44
#define SERVO_RELEASE_DEG 145

#define FAST_BUTTON1 A0
#define FAST_BUTTON2 A1
#define SLOW_BUTTON1 A2
#define SLOW_BUTTON2 A3

#define MAGNET_OUT_ON 6
#define MAGNET_OUT_SWITCH 5
#define MAGNET_IN_ON 7
#define MAGNET_IN_SWITCH 8

#define PUL_PIN 13
#define DIR_PIN 12
#define ENA_PIN 11
#define STEPPER_DELAY_FAST 300
#define STEPPER_DELAY_SLOW 600

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

IcsHardSerialClass krs(&Serial, 2, 115200, 1000);

struct Arm_pos {
  uint32_t hand;
  uint32_t mid;
  uint32_t root;
};

// 手前から
const Arm_pos pos_out[HW] = {
  {ARM_HAND_ZERO,        ARM_MID_ZERO - 4630, ARM_ROOT_ZERO + 1280},
  {ARM_HAND_ZERO + 100,  ARM_MID_ZERO - 4320, ARM_ROOT_ZERO + 1180},
  {ARM_HAND_ZERO + 350,  ARM_MID_ZERO - 4050, ARM_ROOT_ZERO + 1100},
  {ARM_HAND_ZERO + 510,  ARM_MID_ZERO - 3780, ARM_ROOT_ZERO + 1170},
  {ARM_HAND_ZERO + 710,  ARM_MID_ZERO - 3480, ARM_ROOT_ZERO + 1280},
  {ARM_HAND_ZERO + 910,  ARM_MID_ZERO - 3180, ARM_ROOT_ZERO + 1390},
  {ARM_HAND_ZERO + 1170, ARM_MID_ZERO - 2780, ARM_ROOT_ZERO + 1510},
  {ARM_HAND_ZERO + 1400, ARM_MID_ZERO - 2380, ARM_ROOT_ZERO + 1570}
  
};
const Arm_pos pos_in[HW] = {
  
};

void setup() {
  Wire.begin(8);
  Wire.setClock(100000);
  Wire.onReceive(demo_receive);
  flip_servo.attach(9);
  pinMode(FAST_BUTTON1, INPUT_PULLUP);
  pinMode(FAST_BUTTON2, INPUT_PULLUP);
  pinMode(SLOW_BUTTON1, INPUT_PULLUP);
  pinMode(SLOW_BUTTON2, INPUT_PULLUP);
  pinMode(MAGNET_OUT_ON, OUTPUT);
  pinMode(MAGNET_OUT_SWITCH, OUTPUT);
  pinMode(MAGNET_IN_ON, OUTPUT);
  pinMode(MAGNET_IN_SWITCH, OUTPUT);
  pinMode(PUL_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);
  krs.begin();
  flip_servo.write(SERVO_RELEASE_DEG);
  Arm_pos pos;
  for (int i = 0; i < 8; ++i){
    pos = pos_out[i];
    pos.root -= 400;
    move_arm(pos, 30, 10);
    
    pos.root += 400;
    move_arm(pos, 30, 10);
    release_out();
    
    pos.root -= 400;
    move_arm(pos, 30, 10);

    pos.root += 400;
    move_arm(pos, 30, 10);
    hold_black_out();

    pos.root -= 400;
    move_arm(pos, 30, 10);
    
  }
  Arm_pos arm_pos = {ARM_HAND_ZERO + 1400, ARM_MID_ZERO - 2380, ARM_ROOT_ZERO + 1570};
  move_arm(arm_pos, 30, 10);
}

void stepper_enable() {
  digitalWrite(ENA_PIN, HIGH);
}

void stepper_disable() {
  digitalWrite(ENA_PIN, LOW);
}

void stepper_move(bool dir, int steps, int dly) {
  digitalWrite(DIR_PIN, dir);
  for (int i = 0; i < steps; ++i) {
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(dly);
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(dly);
  }
}

void hold_black_out() {
  digitalWrite(MAGNET_OUT_SWITCH, LOW);
  delay(10);
  digitalWrite(MAGNET_OUT_ON, HIGH);
}

void hold_white_out() {
  digitalWrite(MAGNET_OUT_SWITCH, HIGH);
  delay(10);
  digitalWrite(MAGNET_OUT_ON, HIGH);
}

void release_out() {
  digitalWrite(MAGNET_OUT_SWITCH, LOW);
  delay(10);
  digitalWrite(MAGNET_OUT_ON, LOW);
}

void hold_black_in() {
  digitalWrite(MAGNET_IN_SWITCH, LOW);
  delay(10);
  digitalWrite(MAGNET_IN_ON, HIGH);
}

void hold_white_in() {
  digitalWrite(MAGNET_IN_SWITCH, HIGH);
  delay(10);
  digitalWrite(MAGNET_IN_ON, HIGH);
}

void release_in() {
  digitalWrite(MAGNET_IN_SWITCH, LOW);
  delay(10);
  digitalWrite(MAGNET_IN_ON, LOW);
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

void demo() {
  Arm_pos arm_pos = {ARM_HAND_TEST_1, ARM_MID_TEST_1, ARM_ROOT_TEST_1 - 400};
  move_arm(arm_pos, 30, 10);
  stepper_enable();
  stepper_move(true, 2000, STEPPER_DELAY_FAST);
  delay(1000);
  stepper_move(false, 2000, STEPPER_DELAY_FAST);
  stepper_disable();

  arm_pos.root += 400;
  move_arm(arm_pos, 30, 10);

  flip_servo.write(SERVO_RELEASE_DEG);
  hold_black_out();
  delay(500);

  arm_pos.root -= 400;
  move_arm(arm_pos, 30, 10);

  flip_servo.write(SERVO_GRIP_DEG);
  delay(100);
  hold_white_in();
  delay(200);
  release_out();
  delay(300);
  flip_servo.write(SERVO_RELEASE_DEG);
  delay(300);

  arm_pos.mid = ARM_MID_TEST_2;
  arm_pos.hand = ARM_HAND_TEST_2;
  arm_pos.root =  ARM_ROOT_TEST_2 - 200;
  move_arm(arm_pos, 30, 10);

  arm_pos.root =  ARM_ROOT_TEST_2;
  move_arm(arm_pos, 30, 10);

  delay(300);

  release_in();

  arm_pos.root -= 400;
  move_arm(arm_pos, 30, 10);



  arm_pos = {ARM_HAND_TEST_1, ARM_MID_TEST_1, ARM_ROOT_TEST_1};
  move_arm(arm_pos, 30, 10);

  flip_servo.write(SERVO_RELEASE_DEG);
  hold_white_out();
  delay(500);

  arm_pos.root -= 400;
  move_arm(arm_pos, 30, 10);

  flip_servo.write(SERVO_GRIP_DEG);
  delay(100);
  hold_black_in();
  delay(200);
  release_out();
  delay(300);
  flip_servo.write(SERVO_RELEASE_DEG);
  delay(300);

  arm_pos.mid = ARM_MID_TEST_2;
  arm_pos.hand = ARM_HAND_TEST_2;
  arm_pos.root =  ARM_ROOT_TEST_2 - 200;
  move_arm(arm_pos, 30, 10);

  arm_pos.root =  ARM_ROOT_TEST_2;
  move_arm(arm_pos, 30, 10);

  delay(300);

  release_in();

  arm_pos.root -= 400;
  move_arm(arm_pos, 30, 10);
}

#define NOT_RECEIVED -1

int received = NOT_RECEIVED;

void demo_receive() {
  if (Wire.available()) {
    received = Wire.read();
  }
}

void loop() {
  /*
  if (received != NOT_RECEIVED){
    demo();
    received = NOT_RECEIVED;
  }
  */
}
