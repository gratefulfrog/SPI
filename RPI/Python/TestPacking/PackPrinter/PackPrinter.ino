
#include <Arduino.h>

const uint32_t maxu32 = 4294967295;
uint32_t val = maxu32 -1;

/* some data:

val = maxu32 -1;
NbBytes : 4
Byte : 0 : 11111110
Byte : 1 : 11111111
Byte : 2 : 11111111
Byte : 3 : 11111111

So that means the the byte order is
lowest Byte, next lowest byte, next lowest byte, most significant byte

But each byte is in noraml MSBF order!

 */

void setup() {
  Serial.begin(115200);
  uint8_t *bytePtr = (uint8_t*) & val;

  uint8_t nbBytes =  sizeof (val);

  Serial.println (String("NbBytes : ") + String(nbBytes));
  for (int i=0;i<nbBytes;i++){
    Serial.print   (String("Byte : ") + 
                    String(i)         + 
                    String(" : "));
    Serial.println(*(bytePtr+i),BIN);
  }
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
