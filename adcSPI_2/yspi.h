#ifndef YSPI_H
#define YSPI_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <SPI.h>

/** 
 *  YPSI abstract class.
 *  This serves as a defintion of the YSPI interface.
 *  Note: all the methods are const, thus once instatiated, the itnernal state
 *  of the YSPI instance never changes.
 *  Typical Usage of the concrete children classes:
 *  1. instanciation
 *  
 *     YSPI* yspiPtr =  new ChildClass(...);
 *  
 *  2. Transaction:
 *     yspiPtr->beginTransaction();
 *     
 *     uint8_t result = yspiPtr->transfer(outgoingByte);
 *     
 *     yspiPtr->endTransaction();
 *  
 *  3. if needed, control the Slave Select pin value
 *     
 *     yspiPtr->setSS(HIGH);   // set to HIGH
 *     
 *     yspiPtr->setSS(LOW);    // set to LOW
 */
class YSPI{
  public:
    /**
     * A pure virtual method proividing means of setting SS value.
     * @param highLow the value to which SS will be set, either HIGH or LOW.
     */
    virtual void setSS(uint8_t highLow)     const = 0;  
    /**
     * A pure virtual method creates the conditions necessary prior to a transfer.
     */
    virtual void beginTransaction()         const = 0;
    /**
     * A pure virtual method transfers the data and returns the date read.
     * @param data a byte that will be transfered to the slave device.
     * @return the byte that is simultaneously received during the transfer.
     */
    virtual uint8_t transfer (uint8_t data) const = 0;
    /**
     * A pure virtual method creates the conditions necessary after a transfer.
     */
    virtual void endTransaction()           const = 0;
};

class HWSPI : public YSPI{
  protected:
    const uint8_t SS;             /**< Slave Select pin ID */
    const SPISettings settings;   /**< SPI Settings struct as per SPI library */
    
  public:
     /**
     * Class constructor.
     * @param SSPin the Slave Select pin ID
     * @param clockF the clock frequency to us
     * @param bitOrder either MSBFIRST or LSBFIRST
     * @param dataMode theSPI mode to use eg SPI_MODE0
     * @return an instance.
     */   
     HWSPI( uint8_t SSpin, 
           uint32_t clockF, 
           uint8_t bitOrder, 
           uint8_t dataMode);
     
    /**
     * @see virtual void setSS(uint8_t highLow) const = 0;
     */
    virtual void setSS(uint8_t highLow)     const;
    /**
     * @see virtual void beginTransaction() const = 0;
     */
    virtual void beginTransaction()         const;
    /**
     * @see virtual uint8_t transfer(uint8_t data) const = 0;
     */
    virtual uint8_t transfer (uint8_t data) const;
    /**
     * @see virtual void endTransaction() const = 0;
     */
    virtual void endTransaction()           const;
};


class USARTSPI : public YSPI{
 protected:
  /** 
   *  usartRegisterStruct references to USART data and control registers.
   *  These registers are indirectly addressed via the references in this struct. 
   *  @see http://www.gammon.com.au/spi#reply3
   */
  struct usartRegisterStruct {
    volatile uint8_t &udr,      /**< USART Data Register */
                     &ucsrA,    /**< USART Control and Status Register A */
                     &ucsrB,    /**< USART Control and Status Register B */
                     &ucsrC;    /**< USART Control and Status Register C */
    volatile uint16_t &ubrr;    /**< USART Baud Rate Register */                 
  };
  /** 
   *  pinRegisterStruct references to USART registers used to control pinMode and pin Value.
   *  These registers come from the MCU datasheets. 
   */
  struct pinRegisterStruct {
    volatile uint8_t &io,   /**< for pinMode setting to OUTPUT  */
                     &set;  /**< for digitalWrite setting to HIGH or LOW  */                  
  };
  
  /**
   * uartSpecificBitValueStruct contains bit ids that are used in the USART operations
   * Examples of bit values and their meaning and usage:
   * UDRE0   // UART Data Register Empty 0 is for USART0 !
   * RXC0    //  Receive Complete Bit, 0 is for USART 0!
   * TXC0    //  Transmit Complete Bit, 0 is for USART 0 !
   * DDD4    // Data Direction bit 'D4' implies 'pin D4', this bit ID depends on the CLK pin used
   * DDD5    // Data Direction bit 'D5' implies 'pin D5' this bit depends on the SS pin used
   * UMSEL00 // USART Mode Select bit 0, 'UMSELn0' implies 'USART Mode Select bit 0 for USART n', this bit depends on which USART channel is used
   * UMSEL01 // USART Mode Select bit 1, 'UMSELn1' implies 'USART Mode Select bit 1 for USART n', this bit depends on which USART channel is used
   * TXEN0   // Transmitter Enable BIT: 0 is for USART 0 !
   * RXEN0   // Receiver Enable BIT: 0 is for USART 0 !
   * PORTD5  // Digital Pin Value Set bit: setting this bit implies pin set to HIGH, unsetting implies pin set to LOW, 'D5' is the SS pin ID this pin ID depends on the SS pin used
   * 
   *   all values are specific to the USART channel and SS pins used. 
   */
  struct uartSpecificBitValueStruct {
    const uint8_t   udre,    /**< UART Data Register Empty bit */ 
                    rxc,     /**<  Receive Complete Bit,       */ 
                    txc,     /**<  Transmit Complete Bit       */ 
                    ddCLK,   /**< Data Direction bit for the specific  CLK pin            */ 
                    ddSS,    /**< Data Direction bit for the specific SS pin              */ 
                    umsel0,  /**< USART Mode Select bit 0 for the specific USART channel  */ 
                    umsel1,  /**< USART Mode Select bit 1 for the specific USART channel  */ 
                    txen,    /**< Transmitter Enable BIT */ 
                    rxen,    /**< Receiver Enable BIT    */ 
                    portSS;  /**< Digital Pin Value Set bit for the specific SS pin       */ 
  };
  /** 
   *  usartRegVec static const member vector of usartRegisterStruct . 
   *  @see  struct usartRegisterStruct
   */
  static const usartRegisterStruct usartRegVec[];
  /** 
   *  pinRegVec static const member vector of pinRegisterStruct . 
   *  @see  struct pinRegisterStruct
   */
  static const pinRegisterStruct  pinRegVec[];
  /** 
   *  specificBitVec static const member vector of uartSpecificBitValueStruct . 
   *  @see  struct uartSpecificBitValueStruct
   */
  static const uartSpecificBitValueStruct specificBitVec[];

  /** 
   *  uID USART ID to wiich the instance refers.
   *  ATmeag328p has only one USART, USART0
   *  ATmeaga2650 has 4 USARTs 0,1,2,3
   */
  const uint8_t uID;
 
 public:
 /** 
   *  nbUARTS static const member variable containing the number of USARTS.
   *  @see const uint8_t uID
   */
  static const uint8_t nbUARTS;
 
    /**
     * Class constructor.
     * @param id the USART ID
     * @see const uint8_t uID;
     * @return an instance.
     */   
  USARTSPI(uint8_t id);
  /**
   * @see virtual void setSS(uint8_t highLow) const = 0;
   */
  virtual void setSS(uint8_t highLow)     const;
  /**
   * @see virtual void beginTransaction() const = 0;
   */  
  virtual void beginTransaction()         const;
  /**
   * @see virtual uint8_t transfer(uint8_t data) const = 0;
   */
  virtual uint8_t transfer (uint8_t data) const;
  /**
   * @see virtual void endTransaction() const = 0;
   */
  virtual void endTransaction()           const;
};
#endif

