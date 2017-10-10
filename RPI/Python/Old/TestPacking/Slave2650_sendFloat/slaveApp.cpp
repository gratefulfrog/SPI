#include "app.h"

SlaveApp::SlaveApp() /* :App() */{
  //Serial.begin(11520);
  
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
  //byte res = response()
  //Serial.println(SPDR,DEC);
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
  outgoing += 0.01;
  if (outgoing >= maxOut){
    outgoing=0.0;
  }
}
    
boolean SlaveApp::isSlaveMsg(byte msg) const{
  return !(msg & (0b1111<<4));
}

byte SlaveApp::response(byte incoming){
  byte res = 0;
  static int i =0;
  
  // if we get a new Type, i.e. 1000 0000, then we start a new send of outgoing
  if (incoming & 0b10000000){
    bytePtr = (uint8_t*) & outgoing;
    res = (*(bytePtr+i))>>4 & 0b1111; // send 1st 1/2 byte of outgoing
  }
  else if (incoming == 0b10000){ // send second 1/2 byte of outgoing
    res = (*(bytePtr+i++) & 0b1111);
  }
  else if (incoming == 0b100000){ // send 1st 1/2 byte of outgoing
    res = (*(bytePtr+i))>>4 & 0b1111; // send 1st 1/2 byte of outgoing
  }
  else { // we got filler 
    incOutgoing();
    i=0;
  }
  return res;
}

