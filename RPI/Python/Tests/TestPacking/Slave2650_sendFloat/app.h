#ifndef APP_H
#define APP_H

#include <Arduino.h>
//#include <SPI.h>

class SlaveApp/* : public App */{
  protected:
    float outgoing = 0;
    const float maxOut = 100.0;
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
