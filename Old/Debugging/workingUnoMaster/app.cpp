#include "app.h"

App::App(): badCounter(0){
  Serial.begin(115200);
}

byte App::transferAndWait (const byte what) const{
  byte res = SPI.transfer (what);
  delayMicroseconds (pauseBetweenSends);
  return res;
} 

void App::serialEvent(){
  #ifdef DEBUG
  Serial.println("App.serialEvent...");
  #endif
}

void App::processReply(uint32_t v, boolean isTime){
  if (isTime){
    Serial.print("Slave time: ");
  }
  else{
    Serial.print("Board ID: ");
  }
  (*pFuncPtrUint32)(v);
}

void App::processReply(timeValStruct_t &tvs){
  if (tvs.aidcid != ADCMGR_NULL_ADC_ID){
    (*pFuncPtrTVS)(tvs,badCounter);
    delay(APP_PAUSE_AFTER_GOOD_TVS);  //what happens during this time?
  }
}


