#include "app.h"

App::App(){
  Serial.begin (115200);
}

void App::outgointMsg(char* buf) const{
  // puts a char[] into the arg buf,

  static int sendCount = 0;
  String s = "Send: " + String(sendCount++) + "\n";
  s.toCharArray(buf,s.length()+1);
}

byte App::transferAndWait (const byte what) const{
  byte res = SPI.transfer (what);
  delayMicroseconds (pauseBetweenSends);
  return res;
} 
