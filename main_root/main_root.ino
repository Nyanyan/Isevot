#include <Wire.h>

void setup() {
  Wire.begin();
  Wire.setClock(100000);
  Serial.begin(115200);
}

void loop() {
  Wire.beginTransmission(8);
  Wire.write(1);
  Wire.endTransmission();
  Serial.println("sent");
  delay(10000);
}
