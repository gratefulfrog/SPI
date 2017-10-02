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

  // now wait for a message from master
  while(isSlaveMsg(SPDR));
  SPDR = response(SPDR);  
  //Serial.println(command);
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
  return msg & 0b1111;
}
    
void SlaveApp::SlaveApp::loop(){
  SPI_SlaveReceive();
  //delayMicroseconds(1);
}


