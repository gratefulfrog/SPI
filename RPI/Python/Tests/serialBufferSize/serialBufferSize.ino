#include <Arduino.h>
#include <HardwareSerial.h>

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println(SERIAL_BUFFER_SIZE);

}

void loop() {
  // put your main code here, to run repeatedly:

}
