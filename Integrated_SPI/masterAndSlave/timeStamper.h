#ifndef TIMESTAMPER_H
#define TIMESTAMPER_H

#include <Arduino.h>
#include "utilities.h"

/** TimeStamper class provides time services
 *  at instanciation, or after by a call to setTime0, the time0, i.e. the reference time that should be called 0 microseconds can be set.
 *  
 *  All members are public, but should not be abused!
 *  
 *  The services are:
 *  
 *  getTimeStamp() returns time in micro secs since t0
 *  
 *  getCompensatedTimeStamp(timeStamp_t uTime) returns the uTime (uncompensated time) minus t0, thus the compensated value.
 *  
 *  setTime0(timeStamp_t t) sets t0 to the arg.
 *  
 *  The class provides a public static instance via the pointer TimeStamper::theTimeStamper. This should be the only instance created.
 */
class TimeStamper {
  protected:
   

  public:
    timeStamp_t t0;  /**< the zero time in microseconds */
    /** constructor
     * @param tInit the value to be used to initialize t0  */
    TimeStamper(timeStamp_t tInit);  
    /** gets the current timestamp, i.e. current time - t0 in microsecs if time is less than t0, resets t0 to 0
     *  @return timeStamp time  */
    timeStamp_t getTimeStamp();  
    /** gets the compensated timestamp of the argument, if the argument is less than t0, resets t0 to 0 micro seconds
     *  @param uTime a time value
     *  @return , uTime- t0 in microsecs */
    timeStamp_t getCompensatedTimeStamp(const timeStamp_t &uTime);
    /** Resets the value of t0 to the arg
     *  @param t the new value of t0
     */
    void setTime0(timeStamp_t t);

    /** pointer to a public instance of TimeStamper for use by anyone wh o needs it.
     *  Only one instance should be needed per slave.  */
    static TimeStamper *theTimeStamper;
};

#endif

