#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <wiringPiSPI.h>

class SPI{
 protected:
  uint8_t SPDR;
  const int channel;

 public:
  SPI(int chan,int speed);
  void transferInPlace(uint8_t &inOut); // not const since SPDR is updated
  uint8_t transfer(const uint8_t what);
  uint8_t getSPDR() const;
};

#endif

  
