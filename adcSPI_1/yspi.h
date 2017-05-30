#ifndef YSPI_H
#define YSPI_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

//class YMSPI public: YGeneralSpi{
class YMSPI {
 protected:
  const byte MSPIM_SCK = 4;
  const byte MSPIM_SS = 5;
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
  
  static const uint8_t nbUARTS =  1;
  static const usartRegisterStruct usartRegVec[nbUARTS];
  static const pinRegisterStruct  pinRegVec[nbUARTS];
  
  const uint8_t uID;
 public:
  YMSPI(uint8_t id);
  uint8_t MSPIMTransfer (uint8_t c);
  void setSS(uint8_t highLow);
  void endTransaction();

};
#endif

