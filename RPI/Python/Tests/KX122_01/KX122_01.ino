#include <stdio.h>

#include "I2C.h"
#include "kx122.h"


const String lables[] = {"\tX : ","\tY : ","\tZ : "};

YKX122 *kVec[2];

void setup(){
  Serial.begin(1000000);  // start serial port at 9600 bps
  while (!Serial) {}
  const byte addrVec[] = {0x1E,0x1F};

  
  
  I2c.begin();
  
  for (int i=0;i<2;i++){
    kVec[i] =  new YKX122(addrVec[i]);
  }
  for (int i=0;i<2;i++){
    Serial.println(String(i) + String(" self test : ") + String(kVec[i]->selftest()));
  }
}

void loop(){
  for (int i=0;i<2;i++){
    char buffer[14];
    sprintf(buffer,"\t0x%X : ", kVec[i]->getAddr());
    Serial.print(buffer);
    for (int j=0;j<3;j++){
      Serial.print(lables[j] + String(kVec[i]->acquireChannel(j)));
    }
  }
  Serial.println();
  delay(200);
}

