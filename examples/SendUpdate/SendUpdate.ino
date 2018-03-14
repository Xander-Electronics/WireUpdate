/*
  Send Update

  Send a new sketch on the I2C bus and flash it on the slave board

  This example code is in the public domain.

  created 8 Dec 2017 by Arturo Guadalupi

  Circuit:
  two SAMD based boards (i.e MKR Zero, MKR 1000, MKR FOX 1200, MKR WAN 1300, MKR GSM 1400)
  connected trough I2C bus.
  The master board has an SD card reader with the new firmware to flash called WIRE.bin
*/

#include <WireUpdate.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Master firmware update using I2C bus");
  WireUpdate.beginMaster(4); //optionally WireUpdate.beginMaster(4, myWire); to specify a Wire different from Wire
  delay(100);
  Serial.println("Sending new firmware");
  switch(WireUpdate.sendUpdate(0xAF, "WIRE.bin")) {
    default: break;
    case OK: Serial.println("Done"); break;
    case FILE_ERROR: Serial.println("Error reading file"); break;
    case SD_ERROR: Serial.println("Error reading SD"); break;
  }
}

void loop() {
  // nothing happens after setup
}
