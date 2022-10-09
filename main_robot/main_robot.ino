#include <Wire.h>
#include <Servo.h>
#include <IcsHardSerialClass.h>

#define ROBOT_ID 1

#if ROBOT_ID == 1
  #include "robot1.h"
#else
  #include "robot2.h"
#endif

Servo flip_servo_out;
Servo flip_servo_in;

#define SERVO_OUT 9
#define SERVO_IN 10

#define HW 8

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

#define DISC_SUPPLIER 4
#define DISC_SUPPLY_GET 4826
#define DISC_SUPPLY_READY 10140
#define DISC_SUPPLY_SHAKE 200


#define ARM_HAND_TEST_1 (ARM_HAND_ZERO + 1000)
#define ARM_MID_TEST_1 (ARM_MID_ZERO - 4000)
#define ARM_ROOT_TEST_1 (ARM_ROOT_ZERO + 500)

#define ARM_HAND_TEST_2 (ARM_HAND_ZERO + 1300)
#define ARM_MID_TEST_2 (ARM_MID_ZERO - 3100)
#define ARM_ROOT_TEST_2 (ARM_ROOT_ZERO + 1000)

#define DEG_90 2600

#define FIRST_STEPPER_STEP 4500
#define STEPPER_STEP 2325

#define DELAY_SLOW 700
#define DELAY_MID_FAST 400
#define DELAY_FAST 350

#define BLACK_HOLD HIGH
#define WHITE_HOLD LOW

#define ARM_MOVE_NUM 25

IcsHardSerialClass krs(&Serial, 2, 115200, 1000);

struct Arm_pos {
  uint32_t hand;
  uint32_t mid;
  uint32_t root;
};

// 手前から
const Arm_pos pos_out[HW] = {
  {ARM_HAND_ZERO + 582,  ARM_MID_ZERO - 4567, ARM_ROOT_ZERO + 850 },
  {ARM_HAND_ZERO + 891,  ARM_MID_ZERO - 4210, ARM_ROOT_ZERO + 853 },
  {ARM_HAND_ZERO + 959,  ARM_MID_ZERO - 3934, ARM_ROOT_ZERO + 977 },
  {ARM_HAND_ZERO + 1171, ARM_MID_ZERO - 3577, ARM_ROOT_ZERO + 1090},
  {ARM_HAND_ZERO + 1368, ARM_MID_ZERO - 3265, ARM_ROOT_ZERO + 1213},
  {ARM_HAND_ZERO + 1574, ARM_MID_ZERO - 2843, ARM_ROOT_ZERO + 1363},
  {ARM_HAND_ZERO + 1751, ARM_MID_ZERO - 2490, ARM_ROOT_ZERO + 1574},
  {ARM_HAND_ZERO + 2005, ARM_MID_ZERO - 2045, ARM_ROOT_ZERO + 1703}
};
const Arm_pos pos_in[HW] = {
  {ARM_HAND_ZERO + 57,   ARM_MID_ZERO - 3928, ARM_ROOT_ZERO + 1053},
  {ARM_HAND_ZERO + 263,  ARM_MID_ZERO - 3627, ARM_ROOT_ZERO + 1184},
  {ARM_HAND_ZERO + 478,  ARM_MID_ZERO - 3318, ARM_ROOT_ZERO + 1335},
  {ARM_HAND_ZERO + 665,  ARM_MID_ZERO - 2900, ARM_ROOT_ZERO + 1486},
  {ARM_HAND_ZERO + 880,  ARM_MID_ZERO - 2504, ARM_ROOT_ZERO + 1674},
  {ARM_HAND_ZERO + 1095, ARM_MID_ZERO - 2049, ARM_ROOT_ZERO + 1892},
  {ARM_HAND_ZERO + 1395, ARM_MID_ZERO - 1417, ARM_ROOT_ZERO + 2199},
  {ARM_HAND_ZERO + 1906, ARM_MID_ZERO - 251,  ARM_ROOT_ZERO + 2785}
};

/*
// 手前から
const Arm_pos pos_out[HW] = {
  {ARM_HAND_ZERO + 0,    ARM_MID_ZERO - 4650, ARM_ROOT_ZERO + 1005},
  {ARM_HAND_ZERO + 100,  ARM_MID_ZERO - 4290, ARM_ROOT_ZERO + 1025},
  {ARM_HAND_ZERO + 350,  ARM_MID_ZERO - 4020, ARM_ROOT_ZERO + 1075},
  {ARM_HAND_ZERO + 510,  ARM_MID_ZERO - 3720, ARM_ROOT_ZERO + 1115},
  {ARM_HAND_ZERO + 710,  ARM_MID_ZERO - 3420, ARM_ROOT_ZERO + 1205},
  {ARM_HAND_ZERO + 910,  ARM_MID_ZERO - 3090, ARM_ROOT_ZERO + 1275},
  {ARM_HAND_ZERO + 1170, ARM_MID_ZERO - 2740, ARM_ROOT_ZERO + 1395},
  {ARM_HAND_ZERO + 1400, ARM_MID_ZERO - 2320, ARM_ROOT_ZERO + 1535}
};
const Arm_pos pos_in[HW] = {
  {ARM_HAND_ZERO + 530,  ARM_MID_ZERO - 3750, ARM_ROOT_ZERO + 1055},
  {ARM_HAND_ZERO + 710,  ARM_MID_ZERO - 3460, ARM_ROOT_ZERO + 1125},
  {ARM_HAND_ZERO + 910,  ARM_MID_ZERO - 3130, ARM_ROOT_ZERO + 1235},
  {ARM_HAND_ZERO + 1170, ARM_MID_ZERO - 2750, ARM_ROOT_ZERO + 1385},
  {ARM_HAND_ZERO + 1400, ARM_MID_ZERO - 2370, ARM_ROOT_ZERO + 1525},
  {ARM_HAND_ZERO + 1600, ARM_MID_ZERO - 1940, ARM_ROOT_ZERO + 1725},
  {ARM_HAND_ZERO + 1800, ARM_MID_ZERO - 1370, ARM_ROOT_ZERO + 1935},
  {ARM_HAND_ZERO + 2200, ARM_MID_ZERO - 400,  ARM_ROOT_ZERO + 2355}
};
*/

const Arm_pos pos_home = {ARM_HAND_ZERO + 200, ARM_MID_ZERO - 4300, ARM_ROOT_ZERO - 400};

const Arm_pos pos_avoid = {ARM_HAND_ZERO + 2000, ARM_MID_ZERO - 3300, ARM_ROOT_ZERO - 1000};

const Arm_pos pos_get = {ARM_HAND_ZERO + 1200, ARM_MID_ZERO - 3440, ARM_ROOT_ZERO + 830};

const Arm_pos pos_zero = {ARM_HAND_ZERO, ARM_MID_ZERO, ARM_ROOT_ZERO};

bool received = false;
bool requested = false;

//Arm_pos global_pos;

void setup() {
  Wire.begin(I2C_ID);
  Wire.setClock(100000);
  Wire.onReceive(receive);
  Wire.onRequest(request);
  flip_servo_out.attach(SERVO_OUT);
  flip_servo_in.attach(SERVO_IN);
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
  flip_servo_out.write(SERVO_OUT_RELEASE_DEG);
  flip_servo_in.write(SERVO_IN_RELEASE_DEG);
  move_arm(pos_avoid, ARM_MOVE_NUM, 10);
  //out_demo();
  //in_demo();
  //flip_demo();
}

void flip(bool is_black) {
  flip_servo_out.write(SERVO_OUT_GRIP_DEG);
  flip_servo_in.write(SERVO_IN_GRIP_DEG);
  delay(300);
  if (is_black)
    hold_white_in();
  else
    hold_black_in();
  delay(50);
  if (is_black)
    hold_white_out();
  else
    hold_black_out();
  /*
  if (is_black)
    release_black_out();
  else
    release_white_out();
  */
  flip_servo_out.write(SERVO_OUT_RELEASE_DEG);
  flip_servo_in.write(SERVO_IN_RELEASE_DEG);
  off_out();
}

void stepper_enable() {
  digitalWrite(ENA_PIN, HIGH);
}

void stepper_disable() {
  digitalWrite(ENA_PIN, LOW);
}

void stepper_move(bool dir, int steps, int dly) {
  digitalWrite(DIR_PIN, dir);
  /*
  const int slope = 100;
  for (int i = 0; i < slope; ++i) {
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(dly + (1000 - dly) * (slope - i));
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(dly + (1000 - dly) * (slope - i));
  }
  */
  for (int i = 0; i < steps; ++i) {
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(dly);
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(dly);
  }
  /*
  for (int i = 0; i < slope; ++i) {
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(dly + (1000 - dly) * i);
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(dly + (1000 - dly) * i);
  }
  */
}

void hold_black_out() {
  digitalWrite(MAGNET_OUT_SWITCH, BLACK_HOLD);
  delay(10);
  digitalWrite(MAGNET_OUT_ON, HIGH);
  delay(100);
}

void hold_white_out() {
  digitalWrite(MAGNET_OUT_SWITCH, WHITE_HOLD);
  delay(10);
  digitalWrite(MAGNET_OUT_ON, HIGH);
  delay(100);
}

void release_black_out() {
  off_out();
  /*
  digitalWrite(MAGNET_OUT_ON, LOW);
  delay(10);
  digitalWrite(MAGNET_OUT_SWITCH, WHITE_HOLD);
  delay(10);
  digitalWrite(MAGNET_OUT_ON, HIGH);
  delay(10);
  */
}

void release_white_out() {
  off_out();
  /*
  digitalWrite(MAGNET_OUT_ON, LOW);
  delay(10);
  digitalWrite(MAGNET_OUT_SWITCH, BLACK_HOLD);
  delay(10);
  digitalWrite(MAGNET_OUT_ON, HIGH);
  delay(10);
  */
}

void off_out(){
  digitalWrite(MAGNET_OUT_ON, LOW);
  digitalWrite(MAGNET_OUT_SWITCH, LOW);
}

void hold_black_in() {
  digitalWrite(MAGNET_IN_SWITCH, BLACK_HOLD);
  delay(10);
  digitalWrite(MAGNET_IN_ON, HIGH);
  delay(100);
}

void hold_white_in() {
  digitalWrite(MAGNET_IN_SWITCH, WHITE_HOLD);
  delay(10);
  digitalWrite(MAGNET_IN_ON, HIGH);
  delay(100);
}

void release_black_in() {
  off_in();
  /*
  digitalWrite(MAGNET_IN_ON, LOW);
  delay(10);
  digitalWrite(MAGNET_IN_SWITCH, WHITE_HOLD);
  delay(10);
  digitalWrite(MAGNET_IN_ON, HIGH);
  delay(10);
  */
}

void release_white_in() {
  off_in();
  /*
  digitalWrite(MAGNET_IN_ON, LOW);
  delay(10);
  digitalWrite(MAGNET_IN_SWITCH, BLACK+_HOLD);
  delay(10);
  digitalWrite(MAGNET_IN_ON, HIGH);
  delay(10);
  */
}

void off_in(){
  digitalWrite(MAGNET_IN_ON, LOW);
  digitalWrite(MAGNET_IN_SWITCH, LOW);
}

int32_t abs32(int32_t x){
  if (x >= 0)
    return x;
  return -x;
}

int32_t max32(int32_t a, int32_t b){
  if (a >= b)
    return a;
  return b;
}

int32_t min32(int32_t a, int32_t b){
  if (a <= b)
    return a;
  return b;
}

void move_arm(int32_t deg_hand, int32_t deg_mid, int32_t deg_root, int32_t num, int sleep) {
  deg_hand = max32(3500, min32(11500, deg_hand));
  deg_mid = max32(3500, min32(11500, deg_mid));
  deg_root = max32(3500, min32(11500, deg_root));
  int32_t hand_now = krs.getPos(ARM_HAND);
  int32_t mid_now = krs.getPos(ARM_MID);
  int32_t root_now = krs.getPos(ARM_ROOT);
  int32_t max_deg = abs32(deg_hand - hand_now);
  max_deg = max32(max_deg, abs32(deg_mid - mid_now));
  max_deg = max32(max_deg, abs32(deg_root - root_now));
  int32_t steps = max32(1, (max_deg + num - 1) / num);
  for (int i = 1; i <= steps; ++i) {
    krs.setPos(ARM_HAND, hand_now * (steps - i) / steps + deg_hand * i / steps);
    krs.setPos(ARM_MID, mid_now * (steps - i) / steps + deg_mid * i / steps);
    krs.setPos(ARM_ROOT, root_now * (steps - i) / steps + deg_root * i / steps);
    delay(sleep);
  }
}

void move_arm(Arm_pos arm_pos, uint32_t num, int sleep) {
  move_arm(arm_pos.hand, arm_pos.mid, arm_pos.root, num, sleep);
}

void arm_zero() {
  move_arm(ARM_HAND_ZERO, ARM_MID_ZERO, ARM_ROOT_ZERO, ARM_MOVE_NUM, 10);
}

/*
  void receive() {
  if (Wire.available()) {
    global_pos.hand = ARM_HAND_ZERO + ((int)Wire.read() * 256 + (int)Wire.read()) - 10000;
    global_pos.mid = ARM_MID_ZERO + ((int)Wire.read() * 256 + (int)Wire.read()) - 10000;
    global_pos.root = ARM_ROOT_ZERO + ((int)Wire.read() * 256 + (int)Wire.read()) - 10000;
    received = true;
  }
  }
*/

int mode; // 0: 補給して黒を置く 1: 補給して黒を置く 2: 黒を白にflip 3: 白を黒にflip
int col;
int row;

void receive() {
  if (Wire.available()) {
    mode = Wire.read();
    col = Wire.read();
    row = Wire.read();
    received = true;
  }
}

void request() {
  Wire.write((byte)received);
}

void get_disc(int *place) {
  flip_servo_out.write(SERVO_OUT_RELEASE_DEG);
  flip_servo_in.write(SERVO_IN_RELEASE_DEG);
  stepper_enable();
  move_arm(pos_home, 50, 10);
  stepper_move(true, FIRST_STEPPER_STEP + STEPPER_STEP * (*place), DELAY_FAST);
  Arm_pos pos = pos_get;
  pos.root -= 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  hold_black_out();
  pos.root += 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  delay(100);
  pos = pos_get;
  pos.root -= 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  move_arm(pos_home, ARM_MOVE_NUM, 10);
  stepper_move(false, FIRST_STEPPER_STEP, DELAY_FAST);
  stepper_disable();
  *place = 0;
}

void button_func() {
  stepper_enable();
  bool dir = false;
  int dly = 10;
  while (digitalRead(FAST_BUTTON1) == LOW && digitalRead(FAST_BUTTON2) == HIGH) {
    dir = false;
    dly = DELAY_FAST;
    digitalWrite(DIR_PIN, dir);
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(dly);
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(dly);
  }
  while (digitalRead(FAST_BUTTON1) == HIGH && digitalRead(FAST_BUTTON2) == LOW) {
    dir = true;
    dly = DELAY_FAST;
    digitalWrite(DIR_PIN, dir);
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(dly);
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(dly);
  }
  while (digitalRead(SLOW_BUTTON1) == LOW) {
    dir = false;
    dly = DELAY_SLOW;
    digitalWrite(DIR_PIN, dir);
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(dly);
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(dly);
  }
  while (digitalRead(SLOW_BUTTON2) == LOW) {
    dir = true;
    dly = DELAY_SLOW;
    digitalWrite(DIR_PIN, dir);
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(dly);
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(dly);
  }
  stepper_disable();
}

void put_disc_out(int *place, int put_col, int put_row, bool is_black) {
  stepper_enable();
  if (put_col > (*place))
    stepper_move(false, STEPPER_STEP * (put_col - (*place)), DELAY_FAST);
  else if (put_col < (*place))
    stepper_move(true, STEPPER_STEP * ((*place) - put_col), DELAY_FAST);
  *place = put_col;
  flip_servo_out.write(SERVO_OUT_AVOID_DEG);
  flip_servo_in.write(SERVO_IN_AVOID_DEG);
  Arm_pos pos = pos_out[put_row];
  pos.root -= 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  pos.root += 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  if (is_black)
    release_black_out();
  else
    release_white_out();
  pos.root -= 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  off_out();
  move_arm(pos_home, ARM_MOVE_NUM, 10);
  stepper_disable();
}

void put_disc_in(int *place, int put_col, int put_row, bool is_black) {
  stepper_enable();
  if (put_col > (*place))
    stepper_move(false, STEPPER_STEP * (put_col - (*place)), DELAY_FAST);
  else if (put_col < (*place))
    stepper_move(true, STEPPER_STEP * ((*place) - put_col), DELAY_FAST);
  *place = put_col;
  flip_servo_out.write(SERVO_OUT_AVOID_DEG);
  flip_servo_in.write(SERVO_IN_AVOID_DEG);
  Arm_pos pos = pos_in[put_row];
  pos.root -= 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  pos.root += 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  if (is_black)
    release_black_in();
  else
    release_white_in();
  pos.root -= 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  off_in();
  move_arm(pos_home, ARM_MOVE_NUM, 10);
  stepper_disable();
}

void reset_arm_place(int *place) {
  stepper_enable();
  stepper_move(true, STEPPER_STEP * (*place), DELAY_FAST);
  stepper_disable();
  *place = 0;
}

int place = 0;

void flip_disc_black_to_white(int *place, int put_col, int put_row) {
  stepper_enable();
  if (put_col > (*place))
    stepper_move(false, STEPPER_STEP * (put_col - (*place)), DELAY_FAST);
  else if (put_col < (*place))
    stepper_move(true, STEPPER_STEP * ((*place) - put_col), DELAY_FAST);
  *place = put_col;
  flip_servo_out.write(SERVO_OUT_AVOID_DEG);
  flip_servo_in.write(SERVO_IN_AVOID_DEG);
  Arm_pos pos = pos_out[put_row];
  pos.root -= 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  hold_black_out();
  pos.root += 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  delay(100);
  pos.root -= 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  flip(true);
  flip_servo_out.write(SERVO_OUT_AVOID_DEG);
  flip_servo_in.write(SERVO_IN_AVOID_DEG);
  pos = pos_in[put_row];
  pos.root -= 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  pos.root += 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  release_white_in();
  pos.root -= 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  off_in();
  move_arm(pos_home, ARM_MOVE_NUM, 10);
  stepper_disable();
  flip_servo_out.write(SERVO_OUT_RELEASE_DEG);
  flip_servo_in.write(SERVO_IN_RELEASE_DEG);
}

void flip_disc_white_to_black(int *place, int put_col, int put_row) {
  stepper_enable();
  if (put_col > (*place))
    stepper_move(false, STEPPER_STEP * (put_col - (*place)), DELAY_FAST);
  else if (put_col < (*place))
    stepper_move(true, STEPPER_STEP * ((*place) - put_col), DELAY_FAST);
  *place = put_col;
  flip_servo_out.write(SERVO_OUT_AVOID_DEG);
  flip_servo_in.write(SERVO_IN_AVOID_DEG);
  Arm_pos pos = pos_out[put_row];
  pos.root -= 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  hold_white_out();
  pos.root += 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  delay(100);
  pos.root -= 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  flip(false);
  flip_servo_out.write(SERVO_OUT_AVOID_DEG);
  flip_servo_in.write(SERVO_IN_AVOID_DEG);
  pos = pos_in[put_row];
  pos.root -= 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  pos.root += 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  release_black_in();
  pos.root -= 800;
  move_arm(pos, ARM_MOVE_NUM, 10);
  off_in();
  move_arm(pos_home, ARM_MOVE_NUM, 10);
  stepper_disable();
  flip_servo_out.write(SERVO_OUT_RELEASE_DEG);
  flip_servo_in.write(SERVO_IN_RELEASE_DEG);
}

void avoid(){
  move_arm(pos_avoid, 40, 10);
}

void set_disc(){
  krs.setPos(DISC_SUPPLIER, DISC_SUPPLY_GET);
  delay(800);
  for (int i = 0; i < 5; ++i){
    krs.setPos(DISC_SUPPLIER, DISC_SUPPLY_GET + DISC_SUPPLY_SHAKE);
    delay(100);
    krs.setPos(DISC_SUPPLIER, DISC_SUPPLY_GET - DISC_SUPPLY_SHAKE);
    delay(100);
  }
  krs.setPos(DISC_SUPPLIER, DISC_SUPPLY_GET);
  delay(200);
  krs.setPos(DISC_SUPPLIER, DISC_SUPPLY_READY);
  delay(800);
}

void loop() {
  /*
    if (received){
    Arm_pos reset_pos = {krs.getPos(ARM_HAND), krs.getPos(ARM_MID), ARM_ROOT_ZERO - 200};
    move_arm(reset_pos, 50, 10);
    move_arm(global_pos, 50, 10);
    received = false;
    }
  */
  if (received) {
    if (mode == 0) {
      get_disc(&place);
      put_disc_out(&place, col, row, true);
    } if (mode == 1) {
      get_disc(&place);
      flip(true);
      put_disc_in(&place, col, row, false);
    } else if (mode == 2) {
      flip_disc_black_to_white(&place, col, row);
    } else if (mode == 3) {
      flip_disc_white_to_black(&place, col, row);
    } else if (mode == 4) {
      avoid();
    } else if (mode == 5){
      set_disc();
    }
    received = false;
  }
  button_func();
}
