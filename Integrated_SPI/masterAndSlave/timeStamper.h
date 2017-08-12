#ifndef TIMESTAMPER_H
#define TIMESTAMPER_H

#include <Arduino.h>

typedef uint32_t timeStamp_t;
  
class TimeStamper {
  protected:
    timeStamp_t t0;

  public:
    TimeStamper(timeStamp_t tInit);
    timeStamp_t getTimeStamp() const;
    void setTime0(timeStamp_t t);
};

#endif

