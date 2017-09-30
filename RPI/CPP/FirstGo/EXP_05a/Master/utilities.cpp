#include "utilities.h"

// 2017 09 30: added protection to delayMicroseconds!

using namespace std;

void  delay(time_t secs){
  struct timespec delayTime = {secs,0};
  nanosleep(&delayTime,NULL);
}

void  delayMicroseconds(long microsecs){
  struct timespec delayTime = {0,
			       min(999999999,microsecs*1000)};
  nanosleep(&delayTime,NULL);
}

