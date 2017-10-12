#include "app.h"

using namespace std;

const struct timespec App::slaveProcessingTime = {slaveDelay,0};

App::App():channel(0){}

void App::outgoingMsg(char* buf) const{
  // puts a char[] into the arg buf,

  static int sendCount = 0;
  sprintf(buf, "Send: %d\n", sendCount++);
}

void App::transferAndWait (unsigned char &inOut) const{
  wiringPiSPIDataRW(channel,&inOut, 1);
  nanosleep(&pauseStruct,NULL);
} 
