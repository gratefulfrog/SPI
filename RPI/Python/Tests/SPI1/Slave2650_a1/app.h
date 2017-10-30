#ifndef APP_H
#define APP_H

#include <Arduino.h>
//#include <SPI.h>

class SlaveApp/* : public App */{
  protected:
    byte outgoing = 0;

    // protected methods
    void SPI_SlaveReceive(void);
    void incOutgoing();
    boolean isSlaveMsg(byte msg) const;
    byte response(byte incoming) const;
    
  public:
    SlaveApp();
    void loop();
    virtual void SPI_ISR ();    
};

#endif
