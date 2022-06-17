#include <Servo.h>
#include <IcsHardSerialClass.h>

Servo flip_servo;

#define SERVO_GRIP_DEG 44
#define SERVO_RELEASE_DEG 145

#define BLACK_MAGNET 5
#define WHITE_MAGNET 6

#define EN_PIN 2

#define ARM_HAND 2
#define ARM_MID 1
#define ARM_ROOT 0

#define ARM_HAND_ZERO 7900
#define ARM_MID_ZERO 8600
#define ARM_ROOT_ZERO 7900

#define DEG_90 2600

IcsHardSerialClass krs(&Serial, EN_PIN, 115200, 1000);

struct Arm_pos{
  uint32_t hand;
  uint32_t mid;
  uint32_t root;
};

void setup() {
  flip_servo.attach(9);
  pinMode(BLACK_MAGNET, OUTPUT);
  pinMode(WHITE_MAGNET, OUTPUT);
  krs.begin();
  /*
  krs.setPos(ARM_HAND, ARM_HAND_ZERO);
  krs.setPos(ARM_MID, ARM_MID_ZERO);
  krs.setPos(ARM_ROOT, ARM_ROOT_ZERO);
  move_arm(6000, 6000, 6000, 100, 10);
  */
  flip_servo.write(SERVO_RELEASE_DEG);
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

void move_arm(uint32_t deg_hand, uint32_t deg_mid, uint32_t deg_root, uint32_t num, int sleep){
  deg_hand = max(3500, min(11500, deg_hand));
  deg_mid = max(3500, min(11500, deg_mid));
  deg_root = max(3500, min(11500, deg_root));
  uint32_t hand_now = krs.getPos(ARM_HAND);
  uint32_t mid_now = krs.getPos(ARM_MID);
  uint32_t root_now = krs.getPos(ARM_ROOT);
  for (int i = 1; i <= num; ++i){
    krs.setPos(ARM_HAND, hand_now * (num - i) / num + deg_hand * i / num);
    krs.setPos(ARM_MID, mid_now * (num - i) / num + deg_mid * i / num);
    krs.setPos(ARM_ROOT, root_now * (num - i) / num + deg_root * i / num);
    delay(sleep);
  }
}

void move_arm(Arm_pos arm_pos, uint32_t num, int sleep){
  move_arm(arm_pos.hand, arm_pos.mid, arm_pos.root, num, sleep);
}

void arm_zero() {
  move_arm(ARM_HAND_ZERO, ARM_MID_ZERO, ARM_ROOT_ZERO, 100, 10);
}


void loop() {
  //arm_zero();
  //return;
  Arm_pos arm_pos = {ARM_HAND_ZERO + DEG_90, ARM_MID_ZERO, ARM_ROOT_ZERO + DEG_90};
  move_arm(arm_pos, 50, 10);
  
  flip_servo.write(SERVO_RELEASE_DEG);
  hold_black();
  delay(2000);
  
  arm_pos.root = ARM_ROOT_ZERO + DEG_90 - 200;
  arm_pos.mid = ARM_MID_ZERO + 1000;
  move_arm(arm_pos, 50, 10);
  
  flip_servo.write(SERVO_GRIP_DEG);
  delay(300);
  hold_white();
  delay(300);
  flip_servo.write(SERVO_RELEASE_DEG);
  delay(300);
  
  arm_pos.root = ARM_ROOT_ZERO + DEG_90;
  arm_pos.mid = ARM_MID_ZERO;
  move_arm(arm_pos, 50, 10);
  
  release_both();

  arm_pos.root = ARM_ROOT_ZERO + DEG_90 - 200;
  arm_pos.mid = ARM_MID_ZERO + 1000;
  move_arm(arm_pos, 50, 10);
  
  for (;;);
}
