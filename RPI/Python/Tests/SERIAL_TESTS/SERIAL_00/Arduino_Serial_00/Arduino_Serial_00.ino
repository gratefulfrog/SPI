#include <Arduino.h>

#include "stdDefs.h"

u8u32f_struct s = {255,2550,0.250};

#define BUFFSIZE  (100)
//u8u32f_struct buffer[BUFFSIZE];

int counter = 255;

void handShake(){
  while (Serial.available() <=0){
  }
  Serial.read();
}

void setup() {
  Serial.begin(1000000);
  while(!Serial);
  handShake();
}

void doBuffer(){
  for (int i = 0; i<BUFFSIZE;i++){
    s.u8++;
    s.u32+=10;
    s.f+=0.001;
    if(! (++counter%256)){
      s.u8  = 0;
      s.u32 = 0;
      s.f   = 0.0;
    }
    Serial.write((uint8_t*)&s,sizeof(s));
  }
}

void loop() {
  
  Serial.write((uint8_t*)&s,sizeof(s));
  
  s.u8++;
  s.u32+=10;
  s.f+=0.001;
  if(! (++counter%256)){
    s.u8  = 0;
    s.u32 = 0;
    s.f   = 0.0; 
  }
  
  delayMicroseconds(500);   // min working value: 100 micros with PC, 500 Iteaduino ith RPI connected, ...@ 2M Baud
  //delay(10);              // min working val on AEM 500 micros at 1M baud, failed at 1.5 Mbaud
}
