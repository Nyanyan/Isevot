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

IcsHardSerialClass krs(&Serial, 2, 115200, 1000);

uint32_t degs[3];

void setup() {
  Wire.begin(8);
  Wire.setClock(100000);
  Wire.onRequest(request);
  flip_servo.attach(9);
  krs.begin();
  flip_servo.write(SERVO_RELEASE_DEG);
}

void send_deg(uint32_t deg) {
  int digits[5];
  for (int i = 0; i < 5; ++i){
    digits[i] = deg % 10;
    deg /= 10;
  }
  for (int i = 4; i >= 0; --i)
    Wire.write('0' + digits[i]);
  Wire.write(' ');
}

void request(){
  for (int i = 0; i < 3; ++i)
    send_deg(degs[i]);
  Wire.write('\n');
}

void loop() {
  degs[0] = krs.getPos(ARM_HAND);
  degs[1] = krs.getPos(ARM_MID);
  degs[2] = krs.getPos(ARM_ROOT);
}
