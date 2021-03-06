#include "app.h"

SlaveApp::SlaveApp() /* :App() */{
  /* Set MISO output, all others input */
  pinMode(MISO,OUTPUT);
  
  /* Enable SPI in SLAVE mode */
  SPCR = (1<<SPE);

#ifdef USE_INTERRUPTS
  // turn on interrupts
  SPCR |= _BV(SPIE);
#endif
}

#ifdef  USE_INTERRUPTS
void SlaveApp::SlaveApp::loop(){
}
#else    
void SlaveApp::SlaveApp::loop(){
  SPI_SlaveReceive();
}
#endif

void SlaveApp::SPI_ISR(){
  if(!isSlaveMsg(SPDR)){
    SPDR = response(SPDR);
  }
}

void SlaveApp::SPI_SlaveReceive(void){
  /* Wait for reception complete */
  while(!(SPSR & (1<<SPIF)));

  // now wait for a message from master
  if(!isSlaveMsg(SPDR)){
    SPDR = response(SPDR);  
  }
}

void SlaveApp::incOutgoing(){
  outgoing = (outgoing+1)%256;
}
    
boolean SlaveApp::isSlaveMsg(byte msg) const{
  return !(msg & (0b1111<<4));
}

byte SlaveApp::response(byte incoming) const{
  byte res = 0;

  // if we get a new Type, i.e. 0 0 0 0 1 0 0 0 , then we start a new send of outgoing
  if (incoming & 0b10000000){
    res = ((outgoing >>4 )& 0b1111);
  }
  else if (incoming == 0b10000){ // send second 1/2 byte of outgoing
    res = (outgoing & 0b1111);
  }
  else { // we got filler 
    incOutgoing();
  }
  return res;
}

