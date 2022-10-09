#include <Wire.h>

void setup() {
  Wire.begin();
  Wire.setClock(100000);
  Serial.begin(115200);
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

void loop() {

  if (Serial.available() >= 3) {
    int nums[3];
    bool err = false;
    for (int i = 0; i < 3; ++i) {
      char elem = Serial.read();
      if (elem == '\n') {
        err = true;
        break;
      }
      nums[i] = (int)elem - (int)'0';
    }
    if (err) {
      Serial.print("19991");
    } else {
      int i2c_id = 0;
      if (nums[0] == 0 || nums[0] == 3 || nums[0] == 4 || nums[0] == 6) {
        i2c_id = 8;
      } else {
        i2c_id = 9;
        nums[2] = 7 - nums[2];
      }
      Wire.beginTransmission(i2c_id);
      if (nums[0] == 5)
        nums[0] = 4;
      if (nums[0] == 6)
        nums[0] = 5;
      Serial.print(i2c_id);
      for (int i = 0; i < 3; ++i)
        Serial.print(nums[i]);
      for (int i = 0; i < 3; ++i)
        Wire.write((byte)nums[i]);
      Wire.endTransmission();
      //Serial.println("sent");
      while (true) {
        Wire.requestFrom(i2c_id, 1);
        if (Wire.read())
          delay(100);
        else
          break;
      }
      char chr = ' ';
      while (chr != '\n')
        chr = Serial.read();
      Serial.print('0');
    }
  }

  //demo();
}
