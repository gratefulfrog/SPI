#ifndef APP_H
#define APP_H

#include <Arduino.h>

#include "config.h"
#include "staticQ.h"
#include "timeStamper.h"
#include "board.h"


// This is the stuct used to handle data read from the sensors
struct u8u32f_struct{
  uint8_t  u8;  // this value encodes the ADC id and the Channel id on a byte, high 4 bits are ADC id, low 4 bits are channel id
  uint32_t u32; // this is the timestamp value
  float    f;   // this is the sensor value
  
  // operators used in comparisons for this struct type
  boolean operator==(const u8u32f_struct& r) const {
    return (u8 == r.u8) &&
           (u32 == r.u32) &&
           (f == r.f);
  }
  boolean operator!=(const u8u32f_struct& r) const {
    return !(*this == r);
  }
} __attribute__((__packed__));  // packing ensures that onlyt the 9 bytes needed by the struct are used in memory

 
class SlaveApp{
  protected:
    /////////////////////////////////////////////
    // definition of the States used in state-machine logic
    /////////////////////////////////////////////
    enum class State  {unstarted,
                       started,
                       initialized,
                       working,
                       readyToSend,
                       sendingStructs};
    /////////////////////////////////////////////
    // Variables only used within main code
    /////////////////////////////////////////////
    State previousState = State::unstarted;
    Q<u8u32f_struct> *q;
    Board *board;
    
    /////////////////////////////////////////////
    // Variables only used within ISR code
    /////////////////////////////////////////////
    const uint32_t init32  = 128,
                   bid32   = 144;
    const uint8_t init8      = 0b10000000,   // DEC 128
                  payload8   = 0b10110000;   // DEC 176
    
    u8u32f_struct *outgoing;
    uint8_t *bytePtr;
    u8u32f_struct initResponseStruct =  {init8, init32,0},
                  payload = {255,255,255.0},  // only used in the SPI_ISR
                  nullStruct = {255,0,0.0};
                  
    /////////////////////////////////////////////
    // variables shared between ISR and main code
    /////////////////////////////////////////////
    volatile State currentState = State::started;
    volatile State nextState = State::unstarted;
    volatile boolean flag = false;
    /////////////////////////////////////////////

    /////////////////////////////////////////////
    // protected ISR methods 
    /////////////////////////////////////////////
    boolean isSlaveMsg(byte msg) const;
    byte response(byte incoming);
    u8u32f_struct* getOutgoing(uint8_t type) ;
    boolean incOutgoing();
    
    /////////////////////////////////////////////
    // protected Main code methods
    /////////////////////////////////////////////
    void setupHBLed();
    void stepHB() const;
    void sayState() const;
    void checkCurrentState() const;
    void createTimeStamper();
    State doWork();
    
  public:
    SlaveApp();
    void loop();
    virtual void SPI_ISR ();    
};

#endif
