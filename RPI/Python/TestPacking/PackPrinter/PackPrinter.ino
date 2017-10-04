
#include <Arduino.h>

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

What about a float?

what about a struct?

 */

struct byteUint32Float {
  uint8_t    b8;
  uint32_t  b32;
  float     f;
} __attribute__((__packed__));

const uint32_t maxu32 = 4294967295;
//uint32_t val = maxu32 -1;

float val = 1.9;

byteUint32Float s = {1,10,1.5};

void setup() {
  Serial.begin(115200);
  uint8_t *bytePtr = (uint8_t*) & s;

  uint8_t nbBytes =  sizeof (s);
  Serial.print("byteUint32Float: ");
  Serial.println(String(s.b8)  + String(", ") + String(s.b32) + String(", ") + String(s.f) );
  Serial.println (String("NbBytes : ") + String(nbBytes));
  for (int i=0;i<nbBytes;i++){
  
    Serial.print   (String("Byte : ") + 
                    String(i)         + 
                    String(" : "));
    Serial.println(*(bytePtr+i),DEC);
  }
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
