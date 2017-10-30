#ifndef APP_H
#define APP_H

#include <Arduino.h>

struct u8u32f_struct{
  uint8_t  u8;
  uint32_t u32;
  float    f;
} __attribute__((__packed__));


class SlaveApp{
  protected:
    u8u32f_struct outgoing = {0,0,0.0};
    const float maxFloat = 10.0;
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
