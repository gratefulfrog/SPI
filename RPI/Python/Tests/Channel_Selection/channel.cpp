#include <stdio.h>
#include <stdint.h>

#include "channel.h"

class Selector{
  public:
  uint8_t cVec[NB_CHANNELS];
  uint8_t currentIndex;
public:
  Selector();
  bool next(uint8_t *res);
};

Selector::Selector(){
  cVec[NB_CHANNELS] = CHANNELS;
  currentIndex =0;
}

bool Selector::next(uint8_t *res){
  bool more = true;
  *res = cVec[currentIndex];
  if(++currentIndex == NB_CHANNELS){
    currentIndex=0;
    more = false;
  }
  return more;
}


int main(){
  Selector s = Selector();
  uint8_t cur;
  bool res;
  do {
    res= s.next(&cur);
    printf("%d\n",cur);
  }
  while (res);
  printf("%u",(unsigned)sizeof(s.cVec));
  return 0;
}
