#include <stdio.h>
#include <iostream>
#include <time.h>

using namespace std;

void outgoingMsg(char* buf) {
  // puts a char[] into the arg buf,
  static int sendCount = 0;
  sprintf(buf, "Send: %d\n", sendCount++);
}


int main(){
  char buff[100];
  const int delay = 1; // second
  const struct timespec pause = {delay,0};

  while(true){
    outgoingMsg(buff);
    cout<< buff;
    nanosleep(&pause,NULL);
  }
}
