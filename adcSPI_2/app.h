#ifndef APP_H
#define APP_H

#include <Arduino.h>
#include "ad7689.h"
#include "yspi.h"


class App{
  protected:  
    const float epsilon    = 0.05;
  
    AD7689 *adc;
    boolean usingUSARTSPI  = true;  // value changed during setup based on reading of yspiOnPin
    const uint8_t adcID;

    static const uint8_t nbChannels = 8;

    virtual void doSelfTest() const = 0;
    virtual YSPI* usartInit()       = 0;
    virtual YSPI* hwInit()          = 0;
    virtual boolean checkChannelReading(uint8_t chan, float reading) const = 0;
    virtual void checkAndTell(uint8_t channel) const = 0;
  public:
    App(uint8_t id) : adcID(id){}
    virtual void runLoop() = 0;
};


class BobTestApp : public App{
  protected:
    const boolean useSerial      = true,
                  showHWSettings = useSerial;
                  
    const uint8_t AD7689_SS_pin = 10,
                  hbPin         = 9,
                  idPin         = 8,
                  truePin       = 7,
                  falsePin      = 6,
                  talkPin       = 3,
                  yspiOnPin     = 2,
                  onTime        = 200,
                  falseOffTime  = 600,
                  trueOffTime   = onTime,
                  nbInfoFlashes = 5;

    void    testSetup();
    void    heartBeat() const;
    void    flash(boolean tf) const;
    void    doSelfTest() const;
    void    flashInfo(int n, bool once = false) const;
    YSPI*   usartInit();
    YSPI*   hwInit();
    boolean checkChannelReading(uint8_t chan, float reading) const;
    void    checkAndTell(uint8_t channel) const;

  public:
    BobTestApp(uint8_t id);
    void runLoop();
};

class YannickTestApp : public App{
  protected:
    const boolean useSerial      = true,
                  showHWSettings = useSerial,
                  talk           = true;
                  
    const uint8_t AD7689_SS_pin  = 10;
    
    static const uint8_t nbUarts = 4;
    static const float correctChannelReadingVec[nbUarts][nbChannels];
    
    void    doSelfTest() const;
    YSPI*   usartInit();
    YSPI*   hwInit();
    float   correctChannelReading(uint8_t chan) const;
    boolean checkChannelReading(uint8_t chan, float reading) const;
    void    checkAndTell(uint8_t channel) const;
  public:
    YannickTestApp(uint8_t id);
    void runLoop();
};
#endif

