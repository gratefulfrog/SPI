#include "app.h"

SlaveApp::SlaveApp() /* :App() */{
  Serial.begin(11520);
  
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
  if (workFlag){
    doWork();
  }
}
#else    
void SlaveApp::SlaveApp::loop(){
  if (workFlag){
    doWork();
  }
  
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

boolean SlaveApp::incOutgoing(){
  // return true if more data is vailable, false otherwise
  boolean res = true;
  if (outgoing->u8 == NB_STRUCTS_2_SEND){
    outgoing->u8 = 0;
    res = false;
  }
  else{
    outgoing->u8++;
    outgoing->u32++;
    outgoing->f += 0.01;
    if (outgoing->f >= maxFloat){
      outgoing->f=0.0;
    }
  }
  return res;
}
    
boolean SlaveApp::isSlaveMsg(byte msg) const{
  return !(msg & (0b1111<<4));
}

u8u32f_struct* SlaveApp::getOutgoing(uint8_t type) {
  if (type == init8){
    return &initResponseStruct;
  }
  else if (type == bid8){
    return &bidResponseStruct;
  }
  else if (type == wakeUp8){
    // stop working and get ready to send all the data back
    workFlag = false;
    return &nullStruct;
  }
  else{
    return &payload;
  }
}

byte SlaveApp::response(uint8_t incoming){
  byte res = 0;
  static int i =0;
  
  // if we get a new Type, i.e. 1000 0000, then we start a new send of outgoing
  if (incoming & 0b10000000){
    outgoing = getOutgoing(incoming);
    bytePtr = (uint8_t*)  outgoing;
    res = (*(bytePtr+i))>>4 & 0b1111; // send 1st 1/2 byte of outgoing
  }
  else if (incoming == 0b10000){ // send second 1/2 byte of outgoing
    res = (*(bytePtr+i++) & 0b1111);
  }
  else if (incoming == 0b100000){ // send 1st 1/2 byte of outgoing
    res = (*(bytePtr+i))>>4 & 0b1111; // send 1st 1/2 byte of outgoing
  }
  else { // we got filler 
    if (outgoing == &payload){
      boolean moreDataAvailable = incOutgoing();
      if (!moreDataAvailable){
        outgoing=&nullStruct;
      }
    }
    else if (outgoing == &nullStruct){
      workFlag = true;
    }
    i=0;
  }
  return res;
}

void SlaveApp::doWork(){
  Serial.println("Working...");
  for (uint32_t i = 0; i < NB_WORK_LOOPS;){
    i++;
  }
}


