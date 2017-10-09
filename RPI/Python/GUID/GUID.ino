
#include <Arduino.h>
#include "guid_24AA025UID.h"

void setup() {
  Serial.begin(115200);
  AA025UID *guid = new AA025UID();
  Serial.print(String("Guid ID : "));
  Serial.println(guid->getGuidID(),HEX);
  delete guid;
}

void loop() {}

