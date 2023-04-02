#include <Wire.h>
#include <IcsHardSerialClass.h>

#define DISC_SUPPLIER 4

IcsHardSerialClass krs(&Serial, 2, 115200, 1000);

uint32_t deg;

void setup() {
  Wire.begin(8);
  Wire.setClock(100000);
  Wire.onRequest(request);
  krs.begin();
  krs.setPos(DISC_SUPPLIER, 7500);
  delay(1000);
  krs.setFree(DISC_SUPPLIER);
}

void send_deg(uint32_t deg) {
  int digits[5];
  for (int i = 0; i < 5; ++i){
    digits[i] = deg % 10;
    deg /= 10;
  }
  for (int i = 4; i >= 0; --i)
    Wire.write('0' + digits[i]);
}

void request(){
  send_deg(deg);
  Wire.write('\n');
}

void loop() {
  deg = krs.setFree(DISC_SUPPLIER);
}
