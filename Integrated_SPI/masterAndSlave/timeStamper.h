#ifndef TIMESTAMPER_H
#define TIMESTAMPER_H

#include <Arduino.h>
#include "utilities.h"
  
class TimeStamper {
  protected:
   

  public:
    timeStamp_t t0;
    TimeStamper(timeStamp_t tInit);
    timeStamp_t getTimeStamp() const;
    timeStamp_t getCompensatedTimeStamp(timeStamp_t uTime) const;
    void setTime0(timeStamp_t t);

    static TimeStamper *theTimeStamper;
};

#endif

