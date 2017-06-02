#ifndef APP_H
#define APP_H

#include <Arduino.h>
#include "ad7689.h"
#include "yspi.h"


class App{  // each instance will manage the test of one ADC
  protected:  
    // static member variable 
    static const uint8_t nbChannels = 8;      // adc number of channels to be tested
    static const float   epsilon    = 0.05;   // used in equality testing

    const boolean useSerial      = true,      // for message output 
                  showHWSettings = useSerial; // inform as to Frequency used in HWSPI

    // member variables assigned during instatiation or execution
    AD7689 *adc;
    boolean usingUSARTSPI  = true;  // value may be changed during setup based on reading of yspiOnPin (BoTestApp only)
    const uint8_t adcID;            // identifying which adc we are testing in this app instance


    virtual void    doSelfTest() const = 0;  // runs the adc self-test and informs, blocks if failure
    virtual YSPI*   usartInit()  const = 0;  // return pointer to USARTSPI instance allocated from the heap
    virtual YSPI*   hwInit()     const = 0;  // return pointer to HWSPI instance allocated from the heap
    virtual boolean checkChannelReading(uint8_t chan,               // returns true if channel reading is as expected, false otherwise
                                        float reading) const = 0;
    virtual void    checkAndTell(uint8_t channel)      const = 0;   // checks the channel tells if it's ok
  
  public:
    virtual void print(String s) const;     // encapsulates Serial.print with the appropriate conditions
    virtual void println(String s) const;   // adds a \n and calls print
    App(uint8_t id);                        // constructor inits the adc ID
    virtual void runLoop() const = 0;       // loops over all the adc channels
};


class BobTestApp : public App{  // class to encapsulate exectuion envt for Bob
  protected:
                  
    const uint8_t AD7689_SS_pin = 10,      // HWSPI SS pin (output)
                  hbPin         = 9,       // heart beat LED pin (output)
                  idPin         = 8,       // ID LED pin (output)
                  truePin       = 7,       // TRUE LED pin (output)
                  falsePin      = 6,       // FALSE LED pin (output)
                  talkPin       = 3,       // used to decide if we will talk (input)
                  yspiOnPin     = 2,       // indicates if we will use usart Spi or not (input)
                  onTime        = 200,     // LED time delay for on (ms)
                  falseOffTime  = 600,     // LED time delay for FALSE off (ms)
                  trueOffTime   = onTime,  // LED time delay for TRUE off (ms)
                  nbInfoFlashes = 5;       // nb of times to flash LED when giving info 

    virtual void print(String s) const;    // local version takes into account Bob test envt.
    void    testSetup()          const;    // sets pinModes
    void    heartBeat()          const;    // flashes heart beat LED
    void    flash(boolean tf)    const;    // flashes appropriately
    void    doSelfTest()         const;    // as per parent class above
    void    flashInfo(int n,               // flash the info LED
                      bool once = false) const;
    YSPI*   usartInit() const ;            // as per parent class above
    YSPI*   hwInit()    const ;            // as per parent class above
    boolean checkChannelReading(uint8_t chan,         // as per parent class above
                                float reading) const;
    void    checkAndTell(uint8_t channel) const;     // as per parent class above

  public:
    BobTestApp(uint8_t id); // instance constructor
    void runLoop() const;   // as per parent class above
};

class YannickTestApp : public App{
  protected:
    // static member variable (ADC, Channel) array of expected values for the ADC's
    static const float correctChannelReadingVec[][nbChannels];  
    
    const boolean talk = true;
                  
    const uint8_t AD7689_SS_pin  = 10;
    
    virtual void print(String s) const;   // local version takes into account Yannick test envt.
    void    doSelfTest()         const;   // as per parent class above
    YSPI*   usartInit()          const;   // as per parent class above
    YSPI*   hwInit()             const;   // as per parent class above
    float   correctChannelReading(uint8_t chan) const;  // as per parent class above
    boolean checkChannelReading(uint8_t chan,           // as per parent class above
                                float reading) const;
    void    checkAndTell(uint8_t channel) const;        // as per parent class above
  public:
    YannickTestApp(uint8_t id);   // instance constructor
    void runLoop() const;         // as per parent class above
};
#endif

