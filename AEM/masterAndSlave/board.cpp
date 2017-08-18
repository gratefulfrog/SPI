#include "board.h"


const uint8_t Board::boardNbOfADCS[] = {BOARD_BOARD_0_NB_ADCS,   // board 0 has 2 adcs
                                        BOARD_BOARD_1_NB_ADCS};   // board 1 has  adc

const timeValStruct_t Board::nullReturn = {ADCMgr::nullADCID,0,0};
    
Board::Board() : nbDataGets(BOARD_OUTPUT_BURST_LENGTH),
                 guid(BOARD_BOARD_ID),
                 nbADCs(Board::boardNbOfADCS[guid]){  
  q = new Q<timeValStruct_t>;
  
  adcMgrVec = new ADCMgr*[nbADCs];
  for (uint8_t  i=0;i<nbADCs;i++){
    boardUSARTChannels[i] = i+1; // init UART channel vec
    adcMgrVec[i] = new YADCMgr(boardUSARTChannels[i],q);  // init adcMgr vec
  } 
}

boardID Board::getGUID() const{
  return guid;
}

void Board::loop(){  
   if (counter<nbADCs){
    adcMgrVec[counter]->runLoop();
  }
  counter = (counter + 1) % BOARD_SLAVE_LOOP_ITERATIONS;
}

timeValStruct_t *Board::pop(){
  noInterrupts();
  timeValStruct_t *tvs = q->pop();
  interrupts();
  static timeValStruct_t res;
  static timeValStruct_t *resPtr;
  if (tvs){
    res.aidcid = tvs->aidcid;
    res.t  = tvs->t;
    res.v  = tvs->v;
    delete tvs;
    resPtr = &res;
    if (!isValid(*resPtr)){
      Serial.println("Bad outgoing TVS!");
    }
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


