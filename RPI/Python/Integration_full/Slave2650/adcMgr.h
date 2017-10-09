#ifndef ADCMGR_H
#define ADCMGR_H

#include <Arduino.h>
#include "config.h"
#include "ad7689.h"
#include "timeStamper.h"
#include "yspi.h"
#include "utilities.h"



/** ADCMgr is the pure abstract class that provides the interface 
 *  to all test running sublcasses.
 *  This is used because Yannick tests on a harvester board, whereas Bob tests
 *  on an Iteaduino Mega.
 *  Configurations differ but share some common methods and variales  */
class ADCMgr{  // each instance will manage the test of one ADC
  protected:  

    // member variables assigned during instatiation or execution
    AD7689 *adc;                     /*!< pointer to instance of ADC that will be created */
    boolean usingUSARTSPI  = true;   /*!< value may be changed during setup based on reading of yspiOnPin */
    const uint8_t adcID;             /*!< identifying which adc (or USART channel) we are testing in this app instance */
    const uint8_t nbChannels;      /*!< adc number of channels to be polled  */

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

  public:
    //static const uint8_t nullADCID  = ADCMGR_NULL_ADC_ID;

    /** get the channle value
     *  @param channel the channel to check and on which to inform
     *  @return the reading obtained
     */                                    
    virtual float getValue(uint8_t channel) const = 0;   
  
    /** App instance constructor inits the adc ID 
     *  @param id  ADC id
     *  @return an instance of App
     */    
    ADCMgr(uint8_t id,uint8_t nbChan);              
};


/** YTestApp concrete sublcass of App
 *  implements test framework on harvester board platform Y */
class YADCMgr : public ADCMgr{
  protected:
    const uint8_t AD7689_SS_pin  = ADCMGR_SS_PIN;    /*!<  HW SPI slave select pin id */

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

    
  public:
    /** YTestApp instance creation as per parent class
     *  @see App::App
     */
    YADCMgr(uint8_t id,uint8_t nbChan);

    /** runLoop  as per parent class
     *  @see App::runLoop
     */
    virtual float getValue(uint8_t channel)const;   
  
};
#endif


