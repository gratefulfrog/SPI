#ifndef APP_H
#define APP_H

#include <Arduino.h>
#include "ad7689.h"
#include "yspi.h"

/** App is the pure abstrace class that provides the interface 
 *  to all test running sublcasses.
 *  This is used because Yannick tests on a harvester board, whereas Bob tests
 *  on an Iteaduino Mega.
 *  Configurations differ but share some common methods and variales
 */

class App{  // each instance will manage the test of one ADC
  protected:  
    // static member variable 
    static const uint8_t nbChannels = 8;      /**< adc number of channels to be tested  */
    static const float   epsilon    = 0.05;   /**< used in equality testing  */

    const boolean useSerial      = true,      /**< for message output  */
                  showHWSettings = useSerial; /**< if true, test will inform as to Frequency used in HWSPI  */

    // member variables assigned during instatiation or execution
    AD7689 *adc;                     /**< pointer to instance of ADC that will be tested */
    boolean usingUSARTSPI  = true;   /**< value may be changed during setup based on reading of yspiOnPin (BobTestApp only) */
    const uint8_t adcID;             /**< identifying which adc (or USART channel) we are testing in this app instance */

    /** doSelfTest pure virtual exectues the ADC self test
     *  runs the adc self-test and informs, blocks if failure  
     */
    virtual void    doSelfTest() const = 0;
    
    /** usartInit pure virtual creates a USARTSPI instance
     *  @return pointer to USARTSPI instance allocated from the heap 
     */
    virtual YSPI*   usartInit()  const = 0;  

    /** hwInit pure virtual creates a HWSPI instance
     *  @return pointer to HWSPI instance allocated from the heap 
     */
    virtual YSPI*   hwInit()     const = 0;  

    /** checkChannelReading pure virtual  
     *  @param chan  the channel to be checked
     *  @param reading the reading that is to be compared to the expected value
     *  @return true if channel reading is as expected, false otherwise 
     */
    virtual boolean checkChannelReading(uint8_t chan,               
                                        float reading) const = 0;
    
    /** checkChannelReading pure virtual checks the channel tells if it's ok
     *  The type of telling depends on the memeber varaibles
     *  @param channel the channel to check and on which to inform
     */                                    
    virtual void    checkAndTell(uint8_t channel)      const = 0;   
  
  public:
    /** print encapsulates Serial.print with the appropriate conditions
     */    
    virtual void print(String s) const;

    /** println encapsulates Serial.println with the appropriate conditions
     */    
    
    virtual void println(String s) const;   
    
    /** App instance constructor inits the adc ID 
     *  @param id  ADC id
     *  @return an instance of App
     */    
    App(uint8_t id);              

    /** runLoop pure virtual loops over all the adc channels
     */              
    virtual void runLoop() const = 0;       
};

/** BobTestApp concrete sublcass of App
 *  implements test framework on Bob's platform
 */
class BobTestApp : public App{  // class to encapsulate exectuion envt for Bob
  protected:
                  
    const uint8_t AD7689_SS_pin = 10,      /**< HWSPI SS pin (output)                             */
                  hbPin         = 9,       /**< heart Beat pin                                    */
                  idPin         = 8,       /**< ID LED pin (output)                               */
                  truePin       = 7,       /**< TRUE LED pin (output)                             */
                  falsePin      = 11,       /**< FALSE LED pin (output)                            */
                  talkPin       = 4,       /**< used to decide if we will talk (input)            */
                  yspiOnPin     = 2,       /**< indicates if we will use usart Spi or not (input) */
                  onTime        = 200,     /**< LED time delay for on (ms)                        */
                  falseOffTime  = 600,     /**< LED time delay for FALSE off (ms)                 */
                  trueOffTime   = onTime,  /**< LED time delay for TRUE off (ms)                  */
                  nbInfoFlashes = 5;       /**< nb of times to flash LED when giving info         */

    /** print local version takes into account Bob test envt.
     *  @see App::print
     */
    virtual void print(String s) const;

    /** heartBeat flashes heart beat LED
     */
    void    heartBeat()          const;    

    /** testSetup sets pinModes
     */
    void    testSetup()          const;

    /** flash flashes appropriate LED 
     *  @param tf TRUE or FALSE determines what to flash and how
     */
    void    flash(boolean tf)    const;

    /** doSelfTest as per parent class
     *  @see App::doSelfTest
     */
    void    doSelfTest()         const;

    /** flashInfo // flash the info LED
     *  @param n number of times to flash
     *  @param once if true only flash once, otherwise repeate several times the indicated number n
     */
    void    flashInfo(int n,               
                      bool once = false) const;

    /** usartInit as per parent class
     *  @see App::usartInit
     */
    YSPI*   usartInit() const ; 

    /** hwInit as per parent class
     *  @see App::hwInit
     */
    YSPI*   hwInit()    const ;

    /** checkChannelReading as per parent class
     *  @see App::checkChannelReading
     */
    boolean checkChannelReading(uint8_t chan,         
                                float reading) const;
    /** checkAndTell as per parent class
     *  @see App::checkAndTell
     */
    void    checkAndTell(uint8_t channel) const;

  public:
    /** BobTestApp instance creation as per parent class
     *  @see App::App
     */
    BobTestApp(uint8_t id); // instance constructor

    /** runLoop  as per parent class
     *  @see App::runLoop
     */
    void runLoop() const;  
};

/** YannickTestApp concrete sublcass of App
 *  implements test framework on Yannick's harvester board platform
 */
class YannickTestApp : public App{
  protected:
    static const float correctChannelReadingVec[][nbChannels];  /**< Array (ADC, Channel) of expected values for the ADC's */ 
    const boolean talk = true;            /** > talk if true the user will be informed of results, otherwise no output */
    const uint8_t AD7689_SS_pin  = 10;    /**<  HW SPI slave select pin id */
    
    /** print local version takes into account Yannick test envt.
     *  @see App::print
     */
    virtual void print(String s) const;

    /** doSelfTest as per parent class
     *  @see App::doSelfTest
     */
    void    doSelfTest()         const;

    /** usartInit as per parent class
     *  @see App::usartInit
     */
    YSPI*   usartInit()          const;

    
    /** hwInit as per parent class
     *  @see App::hwInit
     */
    YSPI*   hwInit()             const;

    /** checkChannelReading as per parent class
     *  @see App::checkChannelReading
     */
    boolean checkChannelReading(uint8_t chan,
                                float reading) const;
    /** checkAndTell as per parent class
     *  @see App::checkAndTell
     */
    void    checkAndTell(uint8_t channel) const;
    
  public:
    /** YannickTestApp instance creation as per parent class
     *  @see App::App
     */
    YannickTestApp(uint8_t id);

    /** runLoop  as per parent class
     *  @see App::runLoop
     */
    void runLoop() const;
};
#endif

