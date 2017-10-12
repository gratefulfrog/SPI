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
    static const int pauseBetweenSends   = 20,  //10 // microseconds
                     bigBuffSize         = 20, // enough space for a long string
                     slaveProcessingTime = 1; // sec

    void outgoingMsg(char* buf) const;
    uint8_t transferAndWait (const uint8_t what) const;

    SPI *spi;

  public:
    App(int chan, int speed);
    virtual void loop() = 0;
};

 class MasterApp: public App{
  protected:
    char nextChar2Send() const;
  public:
    MasterApp(int ch, int sp);
    void loop();
};


#endif
