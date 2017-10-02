#include "app.h"

SlaveApp::SlaveApp(): App(){
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  
  // turn on SPI in slave mode
  SPCR |= _BV(SPE);

  // now turn on interrupts
  SPI.attachInterrupt();  
  Serial.println("Slave");
}

void SlaveApp::SlaveApp::loop(){
}

void SlaveApp::SPI_ISR(){
  command = SPDR;
  outChar =  command ; // char(command-32);
  SPDR = outChar;
}

