#ifndef YSPI_H
#define YSPI_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <SPI.h>

class YSPI{
  public:
    virtual void setSS(uint8_t highLow)  = 0;
    virtual void beginTransaction()      = 0;
    virtual uint8_t transfer (uint8_t c) = 0;
    virtual void endTransaction()        = 0;

};

class HWSPI : public YSPI{
  protected:
    const uint8_t SS;
    const SPISettings settings;
  public:
    HWSPI( uint8_t SSpin, 
           uint32_t clockF, 
           uint8_t bitOrder, 
           uint8_t dataMode);
    virtual void setSS(uint8_t highLow);
    virtual void beginTransaction();
    virtual uint8_t transfer (uint8_t c);
    virtual void endTransaction();

};


class USARTSPI : public YSPI{
 protected:
  const uint8_t MSPIM_SCK = 4;
  const uint8_t MSPIM_SS  = 5;

  struct usartRegisterStruct {
    volatile uint8_t &udr,
                     &ucsrA,
                     &ucsrB,
                     &ucsrC;
    volatile uint16_t &ubrr;                     
  };
  struct pinRegisterStruct {
    volatile uint8_t &io,
                     &set;                     
  };
  
  /*
   * Meaning of bit values and their usage:
   * UDRE0 // UART Data Register Empty 0 is constant for all USARTs
   * RXC0  //  Receive Complete Bit, 0 is constant for all USARTs
   * TXC0  //  Transmit Complete Bit, 0 is constant for all USARTs
   * DDD4  // Data Direction bit 'D4' implies 'pin D4', this bit ID depends on the CLK pin used
   * DDD5  // Data Direction bit 'D5' implies 'pin D5' this bit depends on the SS pin used
   * UMSEL00 // USART Mode Select bit 0, 'UMSELn0' implies 'USART Mode Select bit 0 for USART n', this bit depends on which USART channel is used
   * UMSEL01 // USART Mode Select bit 1, 'UMSELn1' implies 'USART Mode Select bit 1 for USART n', this bit depends on which USART channel is used
   * TXEN0   // Transmitter Enable BIT: 0 is constant for all UARTS!
   * RXEN0   // Receiver Enable BIT: 0 is constant for all UARTs!
   * PORTD5  // Digital Pin Value Set bit: setting this bit implies pin set to HIGH, unsetting implies pin set to LOW, 'D5' is the SS pin ID this pin ID depends on the SS pin used
   * 
   * So in summary: 
   *   DDxn, DDxm, UMSELn0, UMSELn1, PORTxm are specific to the USART channel, CLK and SS pins used.
   *   all other values are constants and can be used as such.
   */

  struct uartSpecificBitValueStruct {
    const uint8_t   ddCLK,   // Data Direction bit for the specific  CLK pin
                    ddSS,    // Data Direction bit for the specific SS pin
                    umsel0,  // USART Mode Select bit 0 for the specific USART channel 
                    umsel1,  // USART Mode Select bit 1 for the specific USART channel 
                    portSS;  // Digital Pin Value Set bit for the specific SS pin
  };
  
  static const uint8_t nbUARTS =  1;
  static const usartRegisterStruct usartRegVec[nbUARTS];
  static const pinRegisterStruct  pinRegVec[nbUARTS];
  static const uartSpecificBitValueStruct specificBitVec[nbUARTS];
  
  const uint8_t uID;
 public:
  USARTSPI(uint8_t id);
  virtual uint8_t transfer (uint8_t c);
  virtual void beginTransaction();
  virtual void setSS(uint8_t highLow);
  virtual void endTransaction();

};
#endif

