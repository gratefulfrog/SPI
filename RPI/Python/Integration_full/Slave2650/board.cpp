#include "board.h"


boardID Board::getGUID() const{
  //AA025UID g = AA025UID();
  return AA025UID().getGuidID();
}

Board::Board(uint8_t numberADC, uint8_t nbChannelVec[]) : nbADCs(numberADC){  
  
  adcMgrVec = new ADCMgr*[nbADCs];
  for (uint8_t  i=0;i<nbADCs;i++){
    boardUSARTChannels[i] = i+1; // init UART channel vec
    adcMgrVec[i] = new YADCMgr(boardUSARTChannels[i],nbChannelVec[i]);  // init adcMgr vec
  } 
}

