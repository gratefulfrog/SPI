#ifndef APP_H
#define APP_H

#include <Arduino.h>
//#include <SPI.h>

class SlaveApp/* : public App */{
  protected:
    uint8_t outgoing = 0;
    uint8_t *bytePtr;

    // protected methods
    void SPI_SlaveReceive(void);
    void incOutgoing();
    boolean isSlaveMsg(byte msg) const;
    byte response(byte incoming);
    
  public:
    SlaveApp();
    void loop();
    virtual void SPI_ISR ();    
};

#endif
