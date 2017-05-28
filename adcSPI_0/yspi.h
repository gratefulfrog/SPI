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
 public:
  YMSPI(uint8_t id);
  uint8_t MSPIMTransfer (byte c);
  void setSS(int highLow);
  void endTransaction();

};
#endif

