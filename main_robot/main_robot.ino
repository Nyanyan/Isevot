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

#define FIRST_STEPPER_STEP 4850
#define STEPPER_STEP 2250

#define DELAY_SLOW 700
#define DELAY_FAST 290

IcsHardSerialClass krs(&Serial, 2, 115200, 1000);

struct Arm_pos {
  uint32_t hand;
  uint32_t mid;
  uint32_t root;
};

// 手前から
const Arm_pos pos_out[HW] = {
  {ARM_HAND_ZERO + 0,    ARM_MID_ZERO - 4650, ARM_ROOT_ZERO + 1005},
  {ARM_HAND_ZERO + 100,  ARM_MID_ZERO - 4320, ARM_ROOT_ZERO + 1025},
  {ARM_HAND_ZERO + 350,  ARM_MID_ZERO - 4060, ARM_ROOT_ZERO + 1075},
  {ARM_HAND_ZERO + 510,  ARM_MID_ZERO - 3750, ARM_ROOT_ZERO + 1115},
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

const Arm_pos pos_home = {ARM_HAND_ZERO + 200, ARM_MID_ZERO - 4300, ARM_ROOT_ZERO - 200};

const Arm_pos pos_get = {ARM_HAND_ZERO + 1200, ARM_MID_ZERO - 3520, ARM_ROOT_ZERO + 750};

bool received = false;
bool requested = false;

//Arm_pos global_pos;

void setup() {
  Wire.begin(8);
  Wire.setClock(100000);
  Wire.onReceive(receive);
  Wire.onRequest(request);
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
  move_arm(pos_home, 100, 10);
  //out_demo();
  //in_demo();
  //flip_demo();
}
/*
  void flip_demo(){
  Arm_pos pos;
  bool is_black = true;
  for (int i = 0; i < 8; ++i){
    pos = pos_out[i];
    pos.root -= 400;
    move_arm(pos, 50, 10);

    pos.root += 400;
    move_arm(pos, 50, 10);
    if (is_black)
      hold_black_out();
    else
      hold_white_out();

    pos.root -= 800;
    move_arm(pos, 50, 10);

    flip(is_black);
    if (i < 7)
      pos = pos_in[i + 1];
    else
      pos = pos_in[i];
    pos.root -= 400;
    move_arm(pos, 50, 10);

    pos.root += 400;
    move_arm(pos, 50, 10);
    release_in();

    pos.root -= 400;
    move_arm(pos, 50, 10);

    is_black = !is_black;
  }
  move_arm(pos_home, 100, 10);
  }

  void in_demo(){
  Arm_pos pos;
  for (int i = 0; i < 8; ++i){
    pos = pos_in[i];
    pos.root -= 400;
    move_arm(pos, 50, 10);

    pos.root += 400;
    move_arm(pos, 50, 10);
    release_in();

    pos.root -= 400;
    move_arm(pos, 50, 10);

    pos.root += 400;
    move_arm(pos, 50, 10);
    hold_black_in();

    pos.root -= 400;
    move_arm(pos, 50, 10);

  }
  move_arm(pos_in[7], 50, 10);
  release_in();
  move_arm(pos_home, 100, 10);
  }

  void out_demo(){
  Arm_pos pos;
  for (int i = 0; i < 8; ++i){
    pos = pos_out[i];
    pos.root -= 400;
    move_arm(pos, 50, 10);

    pos.root += 400;
    move_arm(pos, 50, 10);
    release_out();

    pos.root -= 400;
    move_arm(pos, 50, 10);

    pos.root += 400;
    move_arm(pos, 50, 10);
    hold_black_out();

    pos.root -= 400;
    move_arm(pos, 50, 10);

  }
  move_arm(pos_out[7], 50, 10);
  release_out();
  move_arm(pos_home, 100, 10);
  }
*/

void flip(bool is_black) {
  flip_servo.write(SERVO_GRIP_DEG);
  delay(300);
  if (is_black)
    hold_white_in();
  else
    hold_black_in();
  delay(50);
  release_out();
  flip_servo.write(SERVO_RELEASE_DEG);
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
  delay(100);
}

void hold_white_out() {
  digitalWrite(MAGNET_OUT_SWITCH, HIGH);
  delay(10);
  digitalWrite(MAGNET_OUT_ON, HIGH);
  delay(100);
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
  delay(100);
}

void hold_white_in() {
  digitalWrite(MAGNET_IN_SWITCH, HIGH);
  delay(10);
  digitalWrite(MAGNET_IN_ON, HIGH);
  delay(100);
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
  stepper_enable();
  move_arm(pos_home, 50, 10);
  stepper_move(true, FIRST_STEPPER_STEP + STEPPER_STEP * (*place), DELAY_FAST);
  move_arm(pos_get, 50, 10);
  delay(100);
  hold_black_out();
  Arm_pos pos = pos_get;
  pos.root -= 400;
  move_arm(pos, 50, 10);
  move_arm(pos_home, 50, 10);
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

void put_disc_out(int *place, int put_col, int put_row) {
  stepper_enable();
  if (put_col > (*place))
    stepper_move(false, STEPPER_STEP * (put_col - (*place)), DELAY_FAST);
  else if (put_col < (*place))
    stepper_move(true, STEPPER_STEP * ((*place) - put_col), DELAY_FAST);
  *place = put_col;
  Arm_pos pos = pos_out[put_row];
  move_arm(pos, 50, 10);
  release_out();
  pos.root -= 800;
  move_arm(pos, 50, 10);
  move_arm(pos_home, 50, 10);
  stepper_disable();
}

void put_disc_in(int *place, int put_col, int put_row) {
  stepper_enable();
  if (put_col > (*place))
    stepper_move(false, STEPPER_STEP * (put_col - (*place)), DELAY_FAST);
  else if (put_col < (*place))
    stepper_move(true, STEPPER_STEP * ((*place) - put_col), DELAY_FAST);
  *place = put_col;
  Arm_pos pos = pos_in[put_row];
  move_arm(pos, 50, 10);
  release_in();
  pos.root -= 800;
  move_arm(pos, 50, 10);
  move_arm(pos_home, 50, 10);
  stepper_disable();
}

void reset_arm_place(int *place) {
  stepper_enable();
  stepper_move(true, STEPPER_STEP * (*place), DELAY_FAST);
  stepper_disable();
  *place = 0;
}

int place = 0;

void demo_get_put() {
  if (digitalRead(FAST_BUTTON1) == LOW && digitalRead(FAST_BUTTON2) == LOW) {
    get_disc(&place);
    put_disc_out(&place, 7, 3);
    reset_arm_place(&place);
  }
}

void flip_disc_black_to_white(int *place, int put_col, int put_row) {
  stepper_enable();
  if (put_col > (*place))
    stepper_move(false, STEPPER_STEP * (put_col - (*place)), DELAY_FAST);
  else if (put_col < (*place))
    stepper_move(true, STEPPER_STEP * ((*place) - put_col), DELAY_FAST);
  *place = put_col;
  Arm_pos pos = pos_out[put_row];
  move_arm(pos, 50, 10);
  hold_black_out();
  pos.root -= 800;
  move_arm(pos, 50, 10);
  flip(true);
  pos = pos_in[put_row];
  pos.root -= 800;
  move_arm(pos, 50, 10);
  pos.root += 800;
  move_arm(pos, 50, 10);
  release_in();
  pos.root -= 800;
  move_arm(pos, 50, 10);
  move_arm(pos_home, 50, 10);
  stepper_disable();
}

void flip_disc_white_to_black(int *place, int put_col, int put_row) {
  stepper_enable();
  if (put_col > (*place))
    stepper_move(false, STEPPER_STEP * (put_col - (*place)), DELAY_FAST);
  else if (put_col < (*place))
    stepper_move(true, STEPPER_STEP * ((*place) - put_col), DELAY_FAST);
  *place = put_col;
  Arm_pos pos = pos_out[put_row];
  move_arm(pos, 50, 10);
  hold_white_out();
  pos.root -= 800;
  move_arm(pos, 50, 10);
  flip(false);
  pos = pos_in[put_row];
  pos.root -= 800;
  move_arm(pos, 50, 10);
  pos.root += 800;
  move_arm(pos, 50, 10);
  release_in();
  pos.root -= 800;
  move_arm(pos, 50, 10);
  move_arm(pos_home, 50, 10);
  stepper_disable();
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
      put_disc_out(&place, col, row);
    } if (mode == 1) {
      get_disc(&place);
      flip(true);
      put_disc_in(&place, col, row);
    } else if (mode == 2) {
      flip_disc_black_to_white(&place, col, row);
    } else if (mode == 3) {
      flip_disc_white_to_black(&place, col, row);
    }
    received = false;
  }
  button_func();
}
