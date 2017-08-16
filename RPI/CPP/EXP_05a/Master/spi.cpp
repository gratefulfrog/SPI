#include "spi.h"

using namespace std;

SPI::SPI(int chan,int speed) : channel(chan), SPDR(0){
  wiringPiSPISetup(channel,speed);
}

void  SPI::transferInPlace(uint8_t &inOut){ // not const since SPDR is updated
  wiringPiSPIDataRW(channel,&inOut, 1);
  SPDR = inOut;
}

uint8_t SPI::transfer(const uint8_t what){
  SPDR = what;
  wiringPiSPIDataRW(channel,&SPDR, 1);
  return SPDR;
}
uint8_t getSPDR() const{
  return SPDR;
}


  
