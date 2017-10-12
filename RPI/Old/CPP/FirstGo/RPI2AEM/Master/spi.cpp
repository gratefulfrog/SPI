#include "spi.h"
#include <iostream>

using namespace std;

SPI::SPI(int chan,int speed) : channel(chan), SPDR(0){
  int fd = wiringPiSPISetup(channel,speed);
  cout << "spi fd: " <<fd << endl;
}

void  SPI::transferInPlace(uint8_t &inOut){ // not const since SPDR is updated
  wiringPiSPIDataRW(channel,&inOut, 1);
  SPDR = inOut;
}

uint8_t SPI::transfer(const uint8_t what){
  SPDR = what;
  //cout << "what: " << what << endl;
  wiringPiSPIDataRW(channel,(unsigned char*) &SPDR,1);
  //cout << "SPDR: " << SPDR << endl;
  return SPDR;
}
uint8_t SPI::getSPDR() const{
  return SPDR;
}


  
