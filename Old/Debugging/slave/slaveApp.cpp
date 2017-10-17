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
  
  TimeStamper::theTimeStamper = new TimeStamper(micros());
  
  board = new Board();
  outBID = board->getGUID();
}

void SlaveApp::loop(){
  static unsigned int maxQ = 0;
  static uint32_t     count = 0;
  if (init){
    if (showInitMsg){
      Serial.print("init t0 : ");
      Serial.println(TimeStamper::theTimeStamper->t0);
      showInitMsg = false;
    }
    board->loop();
    unsigned int qS = board->getQSize();
    if(qS > maxQ){
      maxQ = qS;
      Serial.print("Max Q size: ");
      Serial.println(maxQ);
    }
  }
  else{
    Serial.println("waiting for init call");
  }
  // debug tracing counts the loops
  if(!(count++ % 100000)){
    Serial.print(count);
    Serial.print("   Q size: ");
    Serial.println(board->getQSize());
    //Serial.print("Board ADC ID : ");
    //Serial.println(board->getGUID());
    //Serial.print("outTVS.aid, cid : ");
    //Serial.println(outTVS.aidcid);
    /*
    uint8_t a,c;
    uint8_t acid = outTVS.aidcid;
    decode(acid,a,c);
    Serial.print(a);
    Serial.print(" : ");
    Serial.println(c);
    */
  }
}

void SlaveApp::fillStruct(){
  // NOTE: no allocations or calls to functions that allocate memory, i.e. do not call Serial.print !
  sendI = 0;
  
  switch (command){
    case initChar:
      TimeStamper::theTimeStamper->setTime0(micros());
      board->clearQ();
      
      lim = sizeU32; //sizeof(uint32_t);
      outPtr = (byte *) &TimeStamper::theTimeStamper->t0;
      showInitMsg=init=true;
      break;
    case bidChar:
      lim = sizeBID; // sizeof (boardID);
      outPtr = (byte *) &outBID;
      break;
    default:
      lim = sizeTVS; //sizeof (timeValStruct_t);
      tempTVS = board->pop(); 
      outTVS = tempTVS ? *tempTVS : Board::nullReturn;
      /*
      if(tempTVS){
        outTVS = *tempTVS;
        outTVS.aidcid = tempTVS->aidcid;
        outTVS.t  = tempTVS->t;
        outTVS.v  = tempTVS->v;
       
        outPtr = (byte *) &outTVS;
      }
      else{
        outPtr = (byte *) &Board::nullReturn;
      }
      */
      outPtr = (byte *) &outTVS;
      break;
  } 
  
}

void SlaveApp::SPI_ISR(){
  // NOTE: no allocations or calls to functions that allocate memory, i.e. do not call Serial.print !
  if (SPDR != nullChar) {
    command = SPDR;
    fillStruct();  // will take some time, but can't see how to make it faster because outPtr must be set here!
  }
  if (sendI++<lim){
    SPDR = (*outPtr++);
  }
}


