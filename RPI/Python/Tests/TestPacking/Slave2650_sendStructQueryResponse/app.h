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
    const uint32_t init32  = 128,
                   bid32   = 144;
    const uint8_t init8    = 0b10000000,   // DEC 128
                  bid8     = 0b10010000;   // DEC 144
    u8u32f_struct initResponseStruct =  {init8,init32,0},
                   bidResponseStruct  = {bid8, bid32, 0},
                   payload = {0,0,0.0};
    
    const float maxFloat = 10.0;
    u8u32f_struct *outgoing;

    uint8_t *bytePtr;

    // protected methods
    void SPI_SlaveReceive(void);
    void incOutgoing();
    boolean isSlaveMsg(byte msg) const;
    u8u32f_struct* getOutgoing(uint8_t type) ;
    byte response(byte incoming);
    
  public:
    SlaveApp();
    void loop();
    virtual void SPI_ISR ();    
};

#endif
