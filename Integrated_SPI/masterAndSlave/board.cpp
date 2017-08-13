#include "board.h"


const uint8_t Board::boardNbOfADCS[] = {2};  // board 0 has 2 adcs

const uint8_t Board::boardUSARTChannels[] = {1,2}; // do not usart channel 0

const timeValStruct_t Board::nullReturn = {ADCMgr::nullADCID,0,0};
    

Board::Board(boardID iid) : nbDataGets(OUTPUT_BURST_LENGTH),
                            guid(iid),
                            nbADCs(Board::boardNbOfADCS[guid]){
  q = new Q<timeValStruct_t>;
  
  adcMgrVec = new ADCMgr*[nbADCs];
  for (uint8_t  i=0;i<nbADCs;i++){
    adcMgrVec[i] = new YADCMgr(boardUSARTChannels[i],q);  
  }
}

boardID Board::getGUID() const{
  return guid;
}

void Board::loop(){  
   if (counter<nbADCs){
    adcMgrVec[counter]->runLoop();
  }
  counter = (counter + 1) % SLAVE_LOOP_ITERATIONS;
}

timeValStruct_t *Board::pop(){
  noInterrupts();
  timeValStruct_t *tvs = q->pop();
  interrupts();
  static timeValStruct_t res;
  static timeValStruct_t *resPtr;
  if (tvs){
    res.aidcid = tvs->aidcid;
    //res.cid = tvs->cid;
    res.t  = tvs->t;
    res.v  = tvs->v;
    delete tvs;
    resPtr = &res;
  }
  else {
    resPtr = NULL;
  }
  return  resPtr;
}

void Board::showQSize() const{
  Serial.print("Q nbObjects: ");
  Serial.println(q->qNbObjects());
}

void Board::clearQ(){
  while(pop());
}
unsigned int Board::getQSize() const{
  //Serial.println("getQSize");
  return q->qNbObjects();
}


