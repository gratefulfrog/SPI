#include "app.h"

using namespace std;

void App::outgoingMsg(char* buf) const{
  // puts a char[] into the arg buf,

  static int sendCount = 0;
  sprintf(buf, "Send: %d\n", sendCount++);
}

uint8_t App::transferAndWait (const uint8_t what) const{
  uint8_t res = spi->transfer(what);
  delayMicroseconds(pauseBetweenSends);
  return res;
} 

App::App(int chan, int sped){
  spi = new SPI(chan,sped);
}
