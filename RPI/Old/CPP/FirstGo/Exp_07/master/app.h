#ifndef APP_H
#define APP_H

#include <stdint.h>
#include <stdio.h>
#include <iostream>

#include "spi.h"
#include "utilities.h"
#include "SPI_anything.h"

struct DataStruct{
  char c0, 
    c1, 
    c2;
  int16_t i0;
}__attribute__((__packed__));


class App{
  protected:
    static const int bigBuffSize         = 20, // enough space for a long string
                     slaveProcessingTime = 5;  // seconds

    void outgoingMsg(char* buf) const;
    uint8_t transferAndWait (const uint8_t what) const;

    SPI *spi;

  public:
    static const uint8_t nullChar =  '#';
    static const int pauseBetweenSends   = 5; //10; works  // microseconds
    
    App(int chan, int speed);
    virtual void loop() = 0;
};

class MasterApp: public App{
 protected:
  char nextChar2Send() const,
    outgoing;
  DataStruct inData;
  
 public:
  MasterApp(int ch, int sp);
  void loop();
  virtual void SPI_ISR (){}
};


#endif
