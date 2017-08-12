#include "app.h"

SlaveApp::SlaveApp(): App(){
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  
  // turn on SPI in slave mode
  SPCR |= _BV(SPE);

  // now turn on interrupts
  SPI.attachInterrupt();  
  while(!Serial);
  Serial.println("Slave");
  
  newBoard();
}

void SlaveApp::loop(){
  //Serial.println("Slave loop");
  //static uint32_t time0;
  static uint16_t maxQ =0;
  if (init){
    //Serial.println("about to board loop");
    board->loop();
    if(board->getQSize() > maxQ){
      maxQ = board->getQSize();
      Serial.print("Max Q size: ");
      Serial.println(maxQ);
    }
  }
  /*else if (command == initChar){
    TimeStamper::theTimeStamper->setTime0(micros());
    board->clearQ();

    lim = sizeof(uint32_t);
    outPtr = (byte *) &TimeStamper::theTimeStamper->t0;
    Serial.print("init t0 : ");
    Serial.println(TimeStamper::theTimeStamper->t0,DEC);
    init=true;
    //delay(5000);
  }
  */
  else{
    Serial.println("waiting for init call");
  }
}

void SlaveApp::fillStruct(byte inCar){
  static boardID outBID = board->getGUID();
  static timeValStruct_t outTVS;
  sendI = 0;
  
  switch (inCar){
    case initChar:
      //init = false;
      TimeStamper::theTimeStamper->setTime0(micros());
      board->clearQ();
  
      lim = sizeof(uint32_t);
      outPtr = (byte *) &TimeStamper::theTimeStamper->t0;
      Serial.print("init t0 : ");
      Serial.println(TimeStamper::theTimeStamper->t0);
      init=true;
      break;
    case bidChar:
      lim = sizeof (boardID);
      outPtr = (byte *) &outBID;
      break;
    default:
      lim = sizeof (timeValStruct_t);
      timeValStruct_t *tempTVS = board->pop(); 
      if(tempTVS){
        outTVS.aid = tempTVS->aid;
        outTVS.cid = tempTVS->cid;
        outTVS.t  = tempTVS->t;
        outTVS.v  = tempTVS->v;
        outPtr = (byte *) &outTVS;
      }
      else{
        outPtr = (byte *) &Board::nullReturn;
      }
      break;
  } 
}

void SlaveApp::SPI_ISR(){
  if (SPDR != nullChar) {
    command = SPDR;
    fillStruct(command);  // will take some time, but can't see how to make it faster because outPtr must be set here!
  }
  
  if (sendI++<lim){
    SPDR = (*outPtr++);
  }
}

void SlaveApp::newBoard(){
  const uint8_t bid = 0;
  
  TimeStamper::theTimeStamper = new TimeStamper(micros());
  
  board = new Board(bid);
}


