#include <Wire.h>

#define ARM_HAND_ZERO 7900
#define ARM_MID_ZERO 8600
#define ARM_ROOT_ZERO 7700

void setup() {
  Wire.begin(8);
  Wire.setClock(100000);
  Serial.begin(115200);
}

void loop() {
  if (Wire.available())
    Serial.print((char)Wire.read());
}
