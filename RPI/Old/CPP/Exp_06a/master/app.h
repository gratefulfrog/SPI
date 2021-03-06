#ifndef APP_H
#define APP_H

#include <stdint.h>
#include <stdio.h>
#include <iostream>

#include "spi.h"
#include "utilities.h"
#include "SPI_anything.h"


class App{
  protected:
    static const int bigBuffSize         = 20, // enough space for a long string
                     slaveProcessingTime = 5000; // millisecs

    void outgoingMsg(char* buf) const;
    uint8_t transferAndWait (const uint8_t what) const;

    SPI *spi;

    struct DataStruct{
      char c0, 
           c1, 
           c2;
    };

  public:
    static const uint8_t nullChar =  '#';
    static const int pauseBetweenSends   = 5; //10; works  // microseconds
    
    App(int chan, int speed);
    virtual void loop() = 0;
};

 class MasterApp: public App{
  protected:
    char nextChar2Send() const;
    DataStruct inData;
    
  public:
    MasterApp(int ch, int sp);
    void loop();
    virtual void SPI_ISR (){}
};


#endif
