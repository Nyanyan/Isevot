#include <Wire.h>

#define ARM_HAND_ZERO 7900
#define ARM_MID_ZERO 8600
#define ARM_ROOT_ZERO 7700

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000);
  Serial.println("start!");
}

void loop() {
  Serial.print((int)Serial.available());
  if (Serial.available()) {
    Serial.println("\n");
    Serial.println((char)Serial.read());
    Wire.requestFrom(8, 19);
    Serial.println("requested");
    for (int i = 0; i < 19; ++i)
      Serial.print((char)Wire.read());
  }
}
