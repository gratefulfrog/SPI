#include "board.h"



Board::Board(uint8_t numberADCs, const uint8_t nbChannelVec[]) : nbADCs(numberADCs){  
  adcMgrVec = new ADCMgr*[nbADCs];
  #ifdef DEBUG
    SerialUSB.println("called Board::Board");
    SerialUSB.print("NB ADCs : ");
    SerialUSB.println(numberADCs);
  #endif

  
  for (uint8_t  i=0;i<nbADCs;i++){
    if (nbChannelVec[i]>0){
      adcMgrVec[i] = new YADCMgr(i+1,nbChannelVec[i]);  // init adcMgr vec if there are channels, if not, skip
    }
    else{
      adcMgrVec[i] = NULL;
    }
  }
}

boardID Board::getGUID() const{
  return AA025UID().getGuidID();
}

float Board::getValue(uint8_t adcID, uint8_t channelID) const{
  return adcMgrVec[adcID]->getValue(channelID);
}

uint8_t Board::getMgrNbChannels(uint8_t mgrId) const{
  return adcMgrVec[mgrId]->nbChannels;
}

