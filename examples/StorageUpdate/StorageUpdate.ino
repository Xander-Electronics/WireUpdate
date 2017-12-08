/*
  Storage Update

  Receive a new sketch on the I2C bus and flash it on the board

  This example code is in the public domain.

  created 8 Dec 2017 by Arturo Guadalupi

  Circuit:
  two SAMD based boards (i.e MKR Zero, MKR 1000, MKR FOX 1200, MKR WAN 1300, MKR GSM 1400)
  connected trough I2C bus.
*/

#include <WireUpdate.h>

void setup() {
  Serial.begin(115200);
  if (WireUpdate.beginSlave(0xAF)) {
    Serial.println("Started");
  }
  else {
    Serial.println("An error occurred");
  }
}

void loop() {
  // nothing happens after setup
}
