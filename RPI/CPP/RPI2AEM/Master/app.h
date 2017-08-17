#ifndef APP_H
#define APP_H

#include <stdint.h>
#include <stdio.h>
#include <iostream>

#include "config.h"
#include "SPI_anything.h"
#include "utilities.h"
#include "spi.h"
#include "fileMgr.h"



/** App is the pure abstrace class that provides the interface 
 *  to all concrete sublcasses, e.g. MasterApp and SlaveApp */
class App{
 protected:
  /** used for filtering the replies from the slave */
  static const uint8_t nullADCID = APP_NULL_AIDCID;

  /** pFuncPtrUint32 points to a function that will process any uint32_t values read from the Slave */
  static const processingUint32FuncPtr  pFuncPtrUint32;
    
  /** pFuncPtrTVS points to a function that will process any tvs read from the Slave */
  static const processingTVSFuncPtr pFuncPtrTVS;

  const diskProcessingUint32FuncPtr bidWriterFunc;
  const diskProcessingVoidFuncPtr tidWriterFunc;
  const diskProcessingTVSFuncPtr tvsWriterFunc;

  FileMgr *fm;
    
  /**  initChar,bidChar,acquireChar are character constants for use by both Master and Slave */
  static const unsigned char initChar    = 'i',
    bidChar     = 'b',
    acquireChar = 'a';
  /** bigBuffSize is used to allocate space for Serial printing outputs, */
  static const int bigBuffSize = APP_BIG_BUFF_SIZE, // enough space for a long string
    /**  slaveProcessingTime defines how long to wait to give the slave time to do stuff */                  
    slaveProcessingTime = APP_SLAVE_PROCESSING_TIME; // seconds  
        
  /** transferAndWait SPI transfers a byte and waits pauseBetweenSends microseconds before returning the
   * reply from the transfer which is placed in the argument
   * @param what byte will be sent, 
   * @return the resulting byte
   */
  //void transferAndWait (unsigned char &inOut) const;
  uint8_t transferAndWait (const uint8_t what) const;

  SPI *spi;  /*!< a pointer to an SPI instance used for comms */
    
  /** processReply is overloaded method which will call the functions pointed by pFuncPtrUint32 or pFuncPtrTVS to do the processing
   *  of data read from the Slave. 
   * @param v the value to process,
   * @param isTime a boolean telling if it should be interepreted as a microsecond time value or not
   */
  void processReply(uint32_t v, bool isTime);
  /** processReply is overloaded method which will call the functions pointed by pFuncPtrUint32 or pFuncPtrTVS to do the processing
   *  of data read from the Slave. 
   * @param &tvs is a reference to the timeValStruct_t to be preocessed
   */
  void processReply(timeValStruct_t &tvs);
  

 public:
  /** nullChar is shared betzeen Master and Slave, used as white noise to allow for SPI transfers */
  static const uint8_t nullChar =  SPI_A_NULL_CHAR;
  /** pauseBetweenSends is the microseconds that will be delayed after an SPI transfer of a byte */
  static const int pauseBetweenSends   = APP_PAUSE_BETWEEN_SENDS;  // microseconds
  /** App instance constructor simply turns on Serial output 
   * @param chan  the spi channel to use
   * @param speed the spi speed to use 
   * @param total nb of ADC channels for the board */
  App(int chan, int speed, uint8_t nbADCChannels);
  /** pure virtual loop method will be called repeatedly by the main program */
  virtual void loop() = 0;  
};

/** MasterApp is the concrete class that implementes the Master Funcitonalities.
 * These are : sending init msg to slave, then polling slave for data at the best frequency */
class MasterApp: public App{
 protected:
  /** nextChar2Send method computes the next character to send to the slave 
   * @return the next character.  */
  unsigned char nextChar2Send() const,
    /** outgoing variable contains the character to send out */
    outgoing;
  
  /** readReplyAndSendNext method reads the current reply and then sends the nextCommand
   * @param command the current command for which the method will process the reply
   * @param nextCommand the next command to send.
   */
  void readReplyAndSendNext(char command, char nextCommand);
    
 public:
  /** MasterApp instance constructor as per parent class
   * @param ch  the spi channel to use
   * @param sp the spi speed to use 
   * @param total nb of ADC channels for the board */
  MasterApp(int ch, int sp, uint8_t nbADCChannels);
  
  /** loop as per parent class */
  void loop();        
};

#endif

