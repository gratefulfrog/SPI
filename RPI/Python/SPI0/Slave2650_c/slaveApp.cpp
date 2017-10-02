#include "app.h"

SlaveApp::SlaveApp() /* :App() */{
  Serial.begin(115200);
  /* Set MISO output, all others input */
  pinMode(MISO,OUTPUT);
  //DDR_SPI = (1<<DD_MISO);  // need to set DDR_SPI and DD_MISO to appropriate values from data sheet, good luck!
  /* Enable SPI */
  SPCR = (1<<SPE);

/*
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  
  // turn on SPI in slave mode
  SPCR |= _BV(SPE);

  // turn on interrupts
  SPCR |= _BV(SPIE);
*/
}


void SlaveApp::SPI_SlaveReceive(void){
  /* Wait for reception complete */
  while(!(SPSR & (1<<SPIF)));
  SPDR = 255 - SPDR;
  
  //Serial.println(command);
}

void SlaveApp::SlaveApp::loop(){
  SPI_SlaveReceive();
  //delayMicroseconds(1);
}


