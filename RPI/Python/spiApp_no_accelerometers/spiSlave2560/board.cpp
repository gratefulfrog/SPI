#include "board.h"



Board::Board(uint8_t numberADCs, const uint8_t nbChannelVec[]) : nbADCs(numberADCs){  
  
  adcMgrVec = new ADCMgr*[nbADCs];
  
  for (uint8_t  i=0;i<nbADCs;i++){
    boardUSARTChannels[i] = i+1; // init UART channel vec
    Serial.println(String("init adc_id : ") + 
                   String(i) + 
                   String(" on USART channel : ") + 
                   String(boardUSARTChannels[i]));
    adcMgrVec[i] = new YADCMgr(boardUSARTChannels[i],nbChannelVec[i]);  // init adcMgr vec
    //Serial.println(adcMgrVec[i] == NULL);
  } 
}

boardID Board::getGUID() const{
  //AA025UID g = AA025UID();
  return AA025UID().getGuidID();
}

float Board::getValue(uint8_t adcID, uint8_t channelID) const{
  return adcMgrVec[adcID]->getValue(channelID);
}

uint8_t Board::getMgrNbChannels(uint8_t mgrId) const{
  return adcMgrVec[mgrId]->nbChannels;
}

