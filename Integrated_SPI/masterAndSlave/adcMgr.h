#ifndef ADCMGR_H
#define ADCMGR_H

#include <Arduino.h>
#include "ad7689.h"
#include "q.h"
#include "timeStamper.h"
#include "yspi.h"
#include "utilities.h"

#define NB_CHANNELS (8)  // this may need modification in real life

/** App is the pure abstract class that provides the interface 
 *  to all test running sublcasses.
 *  This is used because Yannick tests on a harvester board, whereas Bob tests
 *  on an Iteaduino Mega.
 *  Configurations differ but share some common methods and variales
 */

#define NULL_ADC_ID (255)

class ADCMgr{  // each instance will manage the test of one ADC
  protected:  
    // static member variable 
    const uint8_t nbChannels;      /**< adc number of channels to be polled  */
    //static const float   epsilon    = 0.05;   /**< used in equality testing  */

    //const boolean useSerial      = false,      /**< for message output  */
    //              showHWSettings = useSerial; /**< if true, test will inform as to Frequency used in HWSPI  */

    // member variables assigned during instatiation or execution
    AD7689 *adc;                     /**< pointer to instance of ADC that will be tested */
    boolean usingUSARTSPI  = true;   /**< value may be changed during setup based on reading of yspiOnPin (BTestApp only) */
    const uint8_t adcID;             /**< identifying which adc (or USART channel) we are testing in this app instance */

    const Q<timeValStruct_t> *q;  /**< pointer to the board's q for pushing data structs onto it */

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
    //virtual boolean checkChannelReading(uint8_t chan,               
    //                                    float reading) const = 0;
    
    /** checkChannelReading pure virtual checks the channel tells if it's ok
     *  The type of telling depends on the memeber varaibles
     *  @param channel the channel to check and on which to inform
     */                                    
    virtual void    checkAndPush(uint8_t channel)      const = 0;   
  
  public:
    static const uint8_t nullADCID  = NULL_ADC_ID;
    /** print encapsulates Serial.print with the appropriate conditions
     */    
    //virtual void print(String s) const;

    /** println encapsulates Serial.println with the appropriate conditions
     */    
    //virtual void println(String s) const;   
    
    /** App instance constructor inits the adc ID 
     *  @param id  ADC id
     *  @return an instance of App
     */    
    ADCMgr(uint8_t id, Q<timeValStruct_t> *q);              

    /** runLoop pure virtual loops over all the adc channels
     */              
    virtual void runLoop() const = 0; 

    //static void serialPrintTVS(timeValStruct_t &tvs);
    //static boolean ADCMgr::isFirstADCCID(timeValStruct_t &tvs);
};


/** YTestApp concrete sublcass of App
 *  implements test framework on harvester board platform Y
 */
class YADCMgr : public ADCMgr{
  protected:
    //static const float correctChannelReadingVec[][nbChannels];  /**< Array (ADC, Channel) of expected values for the ADC's */ 
    //const boolean talk = false;            /** > talk if true the user will be informed of results, otherwise no output */
    const uint8_t AD7689_SS_pin  = 10;    /**<  HW SPI slave select pin id */
    
    /** print local version takes into account Yannick test envt.
     *  @see App::print
     */
    //virtual void print(String s) const;

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
    //boolean checkChannelReading(uint8_t chan,
    //                            float reading) const;
    /** checkAndTell as per parent class
     *  @see App::checkAndTell
     */
    void    checkAndPush(uint8_t channel) const;
    
  public:
    /** YTestApp instance creation as per parent class
     *  @see App::App
     */
    YADCMgr(uint8_t id,Q<timeValStruct_t> *q);

    /** runLoop  as per parent class
     *  @see App::runLoop
     */
    void runLoop() const;
};
#endif


