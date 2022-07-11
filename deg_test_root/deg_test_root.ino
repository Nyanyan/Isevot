#include <Wire.h>

void setup() {
  Wire.begin();
  Wire.setClock(100000);
  Serial.begin(115200);
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
      if (elem == '\n' || elem == '\r'){
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
      for (int i = 0; i < 3; ++i) {
        if (minus[i])
          nums[i] *= -1;
      }
      Wire.beginTransmission(8);
      for (int i = 0; i < 3; ++i)
        nums[i] += 10000;
      for (int i = 0; i < 3; ++i) {
        Wire.write((byte)(nums[i] / 256));
        Wire.write((byte)(nums[i] % 256));
      }
      Wire.endTransmission();
      Serial.println("sent");
    } else {
      Serial.println("err");
    }
  }
}
