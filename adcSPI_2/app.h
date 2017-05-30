#ifndef APP_H
#define APP_H

#include <Arduino.h>
#include "ad7689.h"
#include "yspi.h"


class App{
  protected:
    
  
    const float epsilon = 0.05;
  
    AD7689 *adc;
    YSPI  *yyy;
    uint8_t ch_cnt     = 0; // channel counter
    boolean usingUSARTSPI  = true;  // value changed during setup based on reading of yspiOnPin
    uint32_t timeStamp = 0;    // updated by reads to the adc
  public:
    virtual void runLoop() = 0;
};


class BobTestApp : public App{
  protected:
    const boolean useSerial      = false,
                  showHWSettings = useSerial,
                  talk           = true;
  
    const uint8_t AD7689_SS_pin = 10,
                  hbPin         = 9,
                  idPin         = 8,
                  truePin       = 7,
                  falsePin      = 6,
                  yspiOnPin     = 2,
                  onTime        = 200,
                  falseOffTime  = 600,
                  trueOffTime   = onTime,
                  nbInfoFlashes = 5,
                  nbChannels    = 8;

    void testSetup();
    void heartBeat();
    void flash(boolean tf);
    void doSelfTest();
    void flashInfo(int n, bool once = false);
    void usartInit();
    void hwInit();
    boolean checkChannelReading(int chan, float reading);
    void checkAndTell();

  public:
    BobTestApp();
    virtual void runLoop();
};

class YannickTestApp : public App{
  protected:
    const boolean useSerial      = false,
                  showHWSettings = useSerial,
                  talk           = true;
                  
    const uint8_t AD7689_SS_pin = 10,
                  nbChannels    = 8,
                  usartID       = 1;  // the ID of the USART bus that you will test
    void doSelfTest();
    void usartInit();
    void hwInit();
    boolean checkChannelReading(int chan, float reading);
    void checkAndTell();
  public:
    YannickTestApp();
    virtual void runLoop();
};

#endif

