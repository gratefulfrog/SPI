#include "app.h"

App::App(){}

void App::outgoingMsg(char* buf) const{
  // puts a char[] into the arg buf,

  static int sendCount = 0;
  sprintf(buf, "Send: %d\n", sendCount++);
}

byte App::transferAndWait (const byte what) const{
  uint8_t res = wiringPiSPIDataRW(channel,&what, 1);
  nanosleep(&pauseStruct,NULL);
  return res;
} 
