#include <Arduino.h>

#include "stdDefs.h"
#include "MsgMgr.h"

MsgMgr *comms;

u8u32f_struct s = {0,10,100.0};

char handshakeChar = '|';

void handShake(){
  while (Serial.available() <=0){
  }
}

void setup() {
  Serial.begin(1000000);
  while(!Serial);
  handShake();
  
  comms = new MsgMgr();
}

void loop() {
  static int i=0;
  comms->send(i++);
  /*
  comms->send(&s);
  s.u8++;
  s.u32+=10;
  s.f+=0.01;
  */
  delay(200);
}
