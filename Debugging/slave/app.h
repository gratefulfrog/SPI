#ifndef APP_H
#define APP_H

#include <Arduino.h>
#include <SPI.h>

#include "config.h"
#include "SPI_anything.h"
#include "board.h"
#include "utilities.h"



/** App is the pure abstrace class that provides the interface 
 *  to all concrete sublcasses, e.g. MasterApp and SlaveApp */
class App{
  protected:
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
    static const byte nullChar =  SPI_A_NULL_CHAR;
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


/** SlaveApp class implements the cuntionalities of the MEGA to collect data from the ADCs and respond to commands from the Master */
class SlaveApp: public App{
  protected:
    /** outBid is used to send the Board's ID to the Master. It must be volatile due to itnerrupt handling */
    volatile boardID outBID ;
    /** outTVS is used to send the Board's data to the Master. It must be volatile due to itnerrupt handling */        
    volatile timeValStruct_t outTVS,
    /** tempTVS is used to send the Board's data to the Master. It must be volatile due to itnerrupt handling */        
                            *tempTVS = NULL;
    /** command is used to read from the Master. It must be volatile due to itnerrupt handling */        
    volatile byte command = 0,
    /** outPtr is used to send the data to the Master. It points to the start of the bytes to send. It must be volatile due to itnerrupt handling */   
                  *outPtr;
    /** sendI is the index of the byte to send to the Master. It is incremented from zero and is added to outPtr to get the current byte. It must be volatile due to itnerrupt handling */   
    volatile int   sendI;
    /** init is set to true when the slave has received the initChar from the master. This is a flag used in the slave loop() */
    volatile boolean init = false,
    /** showInitMsg is set to true when the slave has received the initChar from the master. This is a flag used in the slave loop() */
                     showInitMsg = false;

    /** lim contains the current number of bytes to send to the master. */
    volatile unsigned int lim ;
    
    /** board contains a pointer to the master's board instance. The pointer is initialized at SlaveApp instance construction. */
    Board *board;
    
    /** fillStruct method initializes the variables needed to send data back to the master.
     *  It is only called once per command, the multiple bytes of data sent back to the master at each SPI interrupt that is triggerred by a nullChar, 
     *  
     *  A new data send is triggered when a new command is received. */
    void fillStruct();
    
  public:
    /** Instance constructor:  
     *  
     *  sets the MCU to be an SPI slave (HW spi),
     *  
     *  attaches SPI interrupt service routine,
     *  
     *  waits for Serial,
     *  
     *  creates TimeStamper::theTimeStamper,
     *  
     *  creates the board instance,
     *  
     *  assigns the outBID */
    SlaveApp();
    /** This is where it all happens: 
     *  
     *  If not initialized (init member variable), Serial output of wait message;
     *  
     *  otherwsie, if showInitMsg is true, output some init info and set showInitMsg to false, 
     *  
     *  then call the board->loop() and inform if the queue is gettng bigger! */
    void loop();
    /** This method is called when the Slave receives a HW SPI interrupt, i.e. the Master has sent a byte.
     *  
     *  It is important to note that an interrupt handler cannot allocate memory and must be as fast as possible
     *  to not block the main loop. Of course the same apllies to any methods called by the handler.
     *  
     *  This handler checks what is in the SPDR (SPI Data Register).
     *  
     *  If it is not the nullChar, then we know that a new command has come in. So assign the value to the command member variable and call fillStruct() to 
     *  initalize replying
     *  
     *  In all cases, if the send index (sendI) is less than the limit, put the dereferenced value from outPtr into the SPDR and inc the outPtr
     */
    virtual void SPI_ISR ();    
};

#endif

