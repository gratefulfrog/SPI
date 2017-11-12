#include "timeStamper.h"

TimeStamper::TimeStamper(timeStamp_t tInit) : t0(tInit){
  //SerialUSB.println(String("Timestamp t0 : ")+ String(t0));
}
    
timeStamp_t TimeStamper::getTimeStamp(){
  if (micros() < t0){
    t0 = 0;
  }
  return (timeStamp_t) (micros() - t0);
}

timeStamp_t TimeStamper::getCompensatedTimeStamp(const timeStamp_t &uTime){
  if (uTime < t0){
    t0 = 0;
  }
  return (timeStamp_t) (uTime - t0);
}


void TimeStamper::setTime0(timeStamp_t t){
  t0 = t;
}

TimeStamper* TimeStamper::theTimeStamper = NULL;


