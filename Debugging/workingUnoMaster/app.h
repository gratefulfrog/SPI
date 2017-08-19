#ifndef APP_H
#define APP_H

#include <Arduino.h>
#include <SPI.h>

#include "config.h"
#include "SPI_anything.h"
#include "utilities.h"



/** App is the pure abstrace class that provides the interface 
 *  to all concrete sublcasses, e.g. MasterApp and SlaveApp */
class App{
  protected:
    uint32_t badCounter;
    
    /** pFuncPtrUint32 points to a function that will process any uint32_t values read from the Slave */
    static const processingUint32FuncPtr  pFuncPtrUint32 = &serialPrintUint32;
    
    /** pFuncPtrTVS points to a function that will process any tvs read from the Slave */
    static const processingUintTVSFuncPtr pFuncPtrTVS    = &serialPrintTVS;
    
    /**  initChar,bidChar,acquireChar are character constants for use by both Master and Slave */
    static const char initChar    = 'i',
                      bidChar     = 'b',
                      acquireChar = 'a';
    /** bigBuffSize is used to allocate space for Serial printing outputs, */
    static const int bigBuffSize         = APP_BIG_BUFF_SIZE, // enough space for a long string
    /**  slaveProcessingTime defines how long to wait to give the slave time to do stuff */                  
                     slaveProcessingTime = APP_SLAVE_PROCESSING_TIME; // millisecs  with 6 it is too slow for the slave and the slave overfills its q !
    
    /** transferAndWait SPI transfers a byte and waits pauseBetweenSends microseconds before returning the reply from the transfer */
    byte transferAndWait (const byte what) const;

    #ifdef DEBUG
    volatile boolean consoleInput = false;
    #endif
    
    /** processReply is overloaded method which will call the functions pointed by pFuncPtrUint32 or pFuncPtrTVS to do the processing
     *  of data read from the Slave. 
     * @param v the value to process,
     * @param isTime a boolean telling if it should be interepreted as a microsecond time value or not
     */
    void processReply(uint32_t v, boolean isTime);
    /** processReply is overloaded method which will call the functions pointed by pFuncPtrUint32 or pFuncPtrTVS to do the processing
     *  of data read from the Slave. 
     * @param &tvs is a reference to the timeValStruct_t to be preocessed
     */
    void processReply(timeValStruct_t &tvs);
    

  public:
    /** nullChar is shared betzeen Master and Slave, used as white noise to allow for SPI transfers */
    static const byte nullChar =  '#';
    /** pauseBetweenSends is the microseconds that will be delayed after an SPI transfer of a byte */
    static const int pauseBetweenSends   = APP_PAUSE_BETWEEN_SENDS;  // microseconds
    /** App instance constructor simply turns on Serial output */
    App();
    /** pure virtual loop method will be called repeatedly by the main program */
    virtual void loop() = 0;
    /** pure virtual SPI_ISR method will be called at each SPI interrupt, used by Slave only */
    virtual void SPI_ISR () =0;
    /** virtual serialEvent can be overloaded in sublcasses as needed. */
    virtual void serialEvent();
  
};

/** MasterApp is the concrete class that implementes the Master Funcitonalities.
 * These are : sending init msg to slave, then polling slave for data at the best frequency */
class MasterApp: public App{
  protected:
    /** nextChar2Send method computes the next character to send to the slave 
    * @return the next character.  */
    char nextChar2Send() const,
    /** outgoing variable contains the character to send out */
         outgoing;

    /** readReplyAndSendNext method reads the current reply and then sends the nextCommand
    * @param command the current command for which the method will process the reply
    * @param nextCommand the next command to send.
    */
    void readReplyAndSendNext(char command, char nextCommand);
    
  public:
    /** MasterApp instance constructor */
    MasterApp();
    /** loop as per parent class */
    void loop();
    /** as per parent class, empty in MasterApp */
    virtual void SPI_ISR (){}
    #ifdef DEBUG
    virtual void serialEvent();
    #endif
};

#endif

