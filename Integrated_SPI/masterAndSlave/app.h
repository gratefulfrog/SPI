#ifndef APP_H
#define APP_H

#include <Arduino.h>
#include <SPI.h>

#include "SPI_anything.h"
#include "board.h"

// #define DEBUG

class App{
  protected:
    static const int bigBuffSize         = 50, // enough space for a long string
                     slaveProcessingTime = 0; // millisecs  with 6 it is too slow for the slave and the slave overfills its q !

    void printSendCount() const;
    byte transferAndWait (const byte what) const;

    volatile boolean consoleInput = false;

    void printReply(unsigned long &v, boolean isTime);
    void printReply(timeValStruct_t &tvs);
    

  public:
    static const byte nullChar =  '#';
    static const int pauseBetweenSends   = 10;  // microseconds
    
    App();
    virtual void loop() = 0;
    virtual void SPI_ISR () =0;
    virtual void serialEvent();
};

 class MasterApp: public App{
  protected:
    char nextChar2Send() const,
         outgoing;

    void readReply(char command, char nextCommand);
    
  public:
    MasterApp();
    void loop();
    virtual void SPI_ISR (){}
    virtual void serialEvent();
};

class SlaveApp: public App{
  protected:
    volatile byte command = 0;

    volatile byte *outPtr;
    volatile int sendI;
    volatile boolean init = false;

    unsigned int lim ;

    Board *board;
    
    void fillStruct(byte inCar);
    void newBoard();
    
  public:
    SlaveApp();
    void loop();
    virtual void SPI_ISR ();    
};

#endif
