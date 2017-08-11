#include "board.h"


const uint8_t Board::boardNbOfADCS[] = {2};  // board 0 has 2 adcs

const uint8_t Board::boardInformerPin[] = {7}; // board 0 informs on pin 7

const timeValStruct_t Board::nullReturn = {ADCMgr::nullADCID,0,0};
    
/*
Board::Board(boardID iid, 
             int nbSen, 
             Sensor ** sVec) :  nbDataGets(OUTPUT_BURST_LENGTH),
                                guid(iid), 
                                nbSensors(nbSen) {
  sensorVec = sVec;
  q = new Q<timeValStruct_t>;
  ts = new TimeStamper(micros());
}
*/
/*
void Board::updateSensorData(){
  static byte j = 0;
  timeValStruct_t *tvs = new timeValStruct_t;

  sensorVec[j]->getValue(*tvs);
  tvs->t = ts->getTimeStamp();
  noInterrupts();
  q->push(tvs);
  interrupts();
  j = (j+1) % nbSensors;
}
*/
Board::Board(boardID iid) : nbDataGets(OUTPUT_BURST_LENGTH),
                            guid(iid),
                            nbADCs(Board::boardNbOfADCS[guid]),
                            informer(new Informer(Board::boardInformerPin[guid])){
  q = new Q<timeValStruct_t>;
  
  adcMgrVec = new ADCMgr*[nbADCs];
  for (uint8_t  i=0;i<nbADCs;i++){
    adcMgrVec[i] = new YADCMgr(i,q,informer);  
  }
}

void Board::getData(){
  for (int i=0;i< nbDataGets;i++){
    timeValStruct_t *tvs = q->pop();
    if (tvs){   // we got one!!
      ADCMgr::serialPrintTVS(*tvs);  // this is where the tvs data struct should be saved to disk prepended with board ID!!!
      delete tvs;                    // but skip the write if the sensorID is the nullSensorID
    }
    else{
      break;
    }
  }
  Serial.println("EOQ !");
}
boardID Board::getGUID() const{
  return guid;
}

void Board::loop(){  
   if (counter<nbADCs){
    //updateADCData();
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
    res.aid = tvs->aid;
    res.cid = tvs->cid;
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

void Board::setT0(timeStamp_t time0){
  ts->setTime0(time0);
}

void Board::clearQ(){
  while(pop());
}
unsigned int Board::getQSize() const{
  return q->qNbObjects();
}

