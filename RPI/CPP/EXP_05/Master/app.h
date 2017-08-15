#ifndef APP_H
#define APP_H

#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <wiringPiSPI.h>

#include "SPI_anything.h"

class App{
  protected:
    static const int pauseBetweenSends   = 20,  //10 // microseconds
                     bigBuffSize         = 20, // enough space for a long string
                     slaveDelay          = 1; // second
    static const struct timespec slaveProcessingTime;

    static const uint8_t nullChar =  '#';

    void outgoingMsg(char* buf) const;
    void transferAndWait (unsigned char &inOut) const;

    const int channel;

  public:
    App();
    virtual void loop() = 0;
};

 class MasterApp: public App{
  protected:
    char nextChar2Send() const;
  public:
    MasterApp();
    void loop();
};


#endif
