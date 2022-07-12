#include <Wire.h>

#define PUL_PIN 11
#define DIR_PIN 10
#define ENA_PIN 9

#define DELAY_SLOW 12000
#define DELAY_FAST 1200

void setup() {
  Wire.begin();
  Wire.setClock(100000);
  Serial.begin(115200);
  pinMode(PUL_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);
  digitalWrite(ENA_PIN, HIGH);
  digitalWrite(DIR_PIN, HIGH);
}

#define DEMO_N_TASKS 28

int demo_tasks[DEMO_N_TASKS][3] = {
  { -1, -1, -1},
  {0, 2, 3},
  {3, 3, 3},
  { -1, -1, -1},
  {1, 4, 2},
  {2, 4, 3},
  { -1, -1, -1},
  {0, 5, 5},
  {3, 4, 4},
  { -1, -1, -1},
  {1, 4, 5},
  {2, 4, 4},
  { -1, -1, -1},
  {0, 5, 4},
  {3, 4, 4},
  { -1, -1, -1},
  {1, 2, 4},
  {2, 3, 3},
  { -1, -1, -1},
  {0, 2, 2},
  {3, 3, 3},
  { -1, -1, -1},
  {1, 2, 5},
  {2, 3, 4},
  { -1, -1, -1},
  {0, 3, 5},
  {3, 3, 4},
  {3, 4, 5}
};

void demo() {
  if (Serial.available()) {
    for (int i = 0; i < DEMO_N_TASKS; ++i) {
      if (demo_tasks[i][0] == -1) {
        get_new_disc();
      } else {
        Wire.beginTransmission(8);
        for (int j = 0; j < 3; ++j)
          Wire.write((byte)demo_tasks[i][j]);
        Wire.endTransmission();
        while (true) {
          Wire.requestFrom(8, 1);
          if (Wire.read())
            delay(100);
          else
            break;
        }
      }
    }
    while (Serial.available())
      Serial.read();
  }
}

void get_new_disc() {
  for (int i = 0; i < 160; ++i) {
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(DELAY_FAST);
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(DELAY_FAST);
  }
  for (int i = 0; i < 80; ++i) {
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(DELAY_SLOW);
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(DELAY_SLOW);
  }
  for (int i = 0; i < 160; ++i) {
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(DELAY_FAST);
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(DELAY_FAST);
  }
}

void loop() {

  if (Serial.available()) {
    int idx = 0;
    int nums[3] = {0, 0, 0};
    bool minus[3] = {false, false, false};
    while (idx < 3) {
      char elem = Serial.read();
      Serial.print(idx);
      Serial.print('\t');
      Serial.print(elem);
      Serial.print('\t');
      Serial.println((int)elem);
      if (elem == '\n' || elem == '\r') {
        ++idx;
        break;
      }
      if (elem == '-')
        minus[idx] = true;
      else if ('0' <= elem && elem <= '9') {
        nums[idx] *= 10;
        nums[idx] += (int)(elem - '0');
      } else
        ++idx;
    }
    Serial.println(' ');
    while (Serial.available())
      Serial.read();
    Serial.print("received ");
    Serial.print(idx);
    Serial.print(" data\t");
    Serial.print(nums[0]);
    Serial.print('\t');
    Serial.print(nums[1]);
    Serial.print('\t');
    Serial.println(nums[2]);
    if (idx == 3) {
      Wire.beginTransmission(8);
      for (int i = 0; i < 3; ++i)
        Wire.write((byte)nums[i]);
      Wire.endTransmission();
      Serial.println("sent");
    } else {
      get_new_disc();
      Serial.println("get new disc");
    }
  }

  //demo();
}
