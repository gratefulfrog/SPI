#ifndef APP_H
#define APP_H

#include <Arduino.h>
#include "ad7689.h"
#include "yspi.h"

class App{
  protected:  
    const boolean useSerial      = false,
                  showHWSettings = useSerial;
    
    const float epsilon    = 0.05;
                  
    const uint8_t usartID0;  // the ID of the first USART bus that you will test
  
    AD7689 **adcVec;
    uint8_t ch_cnt         = 0, // channel counter
            currentADC     = 0; // ADC counter
    
    boolean usingUSARTSPI  = true;  // value changed during setup based on reading of yspiOnPin
    
    void instantiateADCs();
    virtual void flash(boolean tf) const;
    virtual void doSelfTest(AD7689 *adc) const;
    virtual void flashInfo(int n, bool once = false) const;
    virtual YSPI* usartInit(uint8_t id) const;
    virtual YSPI* hwInit() const;
    
  public:
    App(uint8_t firstUart) : usartID0(firstUart){}
    virtual void runLoop() = 0;
};


class BobTestApp : public App{
  protected:
                  
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
                  nbInfoFlashes = 5,
                  nbChannels    = 8;

    void testSetup() const;
    void heartBeat() const;
    void flash(boolean tf) const;
    void doSelfTest(AD7689 *adc) const;
    void flashInfo(int n, bool once = false) const;
    YSPI* usartInit(uint8_t id) const;
    YSPI* hwInit() const;
    boolean checkChannelReading(int chan, float reading) const;
    void checkAndTell(uint8_t adcID, uint8_t ch) const;

  public:
    BobTestApp(uint8_t firstUart);
    virtual void runLoop();
};

class YannickTestApp : public App{
  protected:
    const boolean talk           = true;
                  
    const uint8_t AD7689_SS_pin  = 10,
                  nbChannels     = 8;

    static const float YannickTestApp::correctVec[][8];
    
    void doSelfTest(AD7689 *adc) const;
    YSPI* usartInit(uint8_t id) const;
    YSPI* hwInit() const;
    float correctChannelReading(uint8_t adcID, uint8_t chan) const;
    boolean checkChannelReading(uint8_t adcID, uint8_t chan, float reading) const;
    void checkAndTell(uint8_t adcID, uint8_t ch) const;
  public:
    YannickTestApp(uint8_t firstUart);
    virtual void runLoop();
};
#endif

