#include <Wire.h>

#define PUL_PIN 11
#define DIR_PIN 10
#define ENA_PIN 9

#define DELAY_SLOW 10000
#define DELAY_FAST 1000

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

#define N_TASKS 12

int tasks[N_TASKS][3] = {
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
  {2, 4, 4}
};

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
  /*
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
  */
  if (Serial.available()) {
    for (int i = 0; i < N_TASKS; ++i) {
      if (tasks[i][0] == -1) {
        get_new_disc();
      } else {
        Wire.beginTransmission(8);
        for (int j = 0; j < 3; ++j)
          Wire.write((byte)tasks[i][j]);
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
