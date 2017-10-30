#include <Arduino.h>

#include "stdDefs.h"

u8u32f_struct s = {255,2550,0.250};

int counter = 255;

void handShake(){
  while (Serial.available() <=0){
  }
}

void setup() {
  Serial.begin(2000000);
  while(!Serial);
  handShake();
}

void loop() {
  static int i=0;
  //comms->send(i++);
  
  //comms->send(&s);  
  Serial.write((uint8_t*)&s,sizeof(s));
  s.u8++;
  s.u32+=10;
  s.f+=0.001;
  if(! (++counter%256)){
    s.u8  = 0;
    s.u32 = 0;
    s.f   = 0.0;
    
  }
  delayMicroseconds(100);   // min working value: 100
}
