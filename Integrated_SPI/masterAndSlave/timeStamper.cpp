#include "timeStamper.h"

TimeStamper::TimeStamper(timeStamp_t tInit) : t0(tInit){}
    
timeStamp_t TimeStamper::getTimeStamp() const{
  return (timeStamp_t) (micros() - t0);
}

timeStamp_t TimeStamper::getCompensatedTimeStamp(timeStamp_t uTime) const{
  return (timeStamp_t) (uTime - t0);
}


void TimeStamper::setTime0(timeStamp_t t){
  t0 = t;
}

TimeStamper* TimeStamper::theTimeStamper = NULL;


