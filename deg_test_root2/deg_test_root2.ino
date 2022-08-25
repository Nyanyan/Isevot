#include <Wire.h>

#define ARM_HAND_ZERO 7900
#define ARM_MID_ZERO 8600
#define ARM_ROOT_ZERO 7700

void setup() {
  Wire.begin();
  Wire.setClock(100000);
  Serial.begin(115200);
}

void receive() {
  Serial.println("receive");
  Serial.print((char)Wire.read());
}

void loop() {
  if (Serial.available()) {
    Serial.read();
    Wire.requestFrom(8, 19);
    for (int i = 0; i < 19; ++i)
      Serial.print((char)Wire.read());
  }
}
