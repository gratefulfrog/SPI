#include "utilities.h"

using namespace std;

void  delay(time_t secs){
  struct timespec delayTime = {secs,0};
  nanosleep(&delayTime,NULL);
}

void  delayMicroseconds(long microsecs){
  struct timespec delayTime = {0,microsecs*1000};
  nanosleep(&delayTime,NULL);
}

