#ifndef APP_H
#define APP_H

#include <Arduino.h>
//#include <SPI.h>

class SlaveApp/* : public App */{
  protected:
    byte command = 0;
    void SPI_SlaveReceive(void);
  public:
    SlaveApp();
    void loop();
    //virtual void SPI_ISR ();    
};

#endif
