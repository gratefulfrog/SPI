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
  if(!isSlaveMsg(SPDR)){
    SPDR = response(SPDR);
  }
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
 
void SlaveApp::incOutgoing(){
  outgoing = (outgoing+1)%256;
}
    
boolean SlaveApp::isSlaveMsg(byte msg) const{
  return !(msg & (0b1111<<4));
}
