#include "app.h"

SlaveApp::SlaveApp() /* :App() */{
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  
  // turn on SPI in slave mode
  SPCR |= _BV(SPE);

  // turn on interrupts
  SPCR |= _BV(SPIE);
  // or use the following...
  // now turn on interrupts
  //SPI.attachInterrupt();  
  
}

void SlaveApp::SlaveApp::loop(){
}

void SlaveApp::SPI_ISR(){
  SPDR = 255 - SPDR;  // only meaningful with numbers
}

