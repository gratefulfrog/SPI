#ifndef APP_H
#define APP_H

#include <Arduino.h>
#include <SPI.h>
//#include "SPI_anything.h"

class App{
  protected:
    static const int pauseBetweenSends   = 20,  //10 // microseconds
                     bigBuffSize         = 20, // enough space for a long string
                     slaveProcessingTime = 1000; // millisecs
    static const byte nullChar =  '#';

    void outgointMsg(char* buf) const;
    byte transferAndWait (const byte what) const;

  public:
    App();
    virtual void loop() = 0;
    virtual void SPI_ISR ()=0;
};

class SlaveApp: public App{
  protected:
    volatile byte command = 0,
                  flag    = 0;
    volatile char outChar = nullChar;
    
  public:
    SlaveApp();
    void loop();
    virtual void SPI_ISR ();    
};

#endif
