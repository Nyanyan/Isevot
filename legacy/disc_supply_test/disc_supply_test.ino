#include <Wire.h>
#include <IcsHardSerialClass.h>

#define DISC_SUPPLIER 4
#define DISC_SUPPLY_GET 4826
#define DISC_SUPPLY_READY 10140
#define DISC_SUPPLY_SHAKE 200

IcsHardSerialClass krs(&Serial, 2, 115200, 1000);

void setup() {
  krs.begin();
}

void loop() {
  krs.setPos(DISC_SUPPLIER, DISC_SUPPLY_GET);
  delay(800);
  for (int i = 0; i < 5; ++i){
    krs.setPos(DISC_SUPPLIER, DISC_SUPPLY_GET + DISC_SUPPLY_SHAKE);
    delay(100);
    krs.setPos(DISC_SUPPLIER, DISC_SUPPLY_GET - DISC_SUPPLY_SHAKE);
    delay(100);
  }
  krs.setPos(DISC_SUPPLIER, DISC_SUPPLY_GET);
  delay(200);
  krs.setPos(DISC_SUPPLIER, DISC_SUPPLY_READY);
  delay(800);
}
