#include "processingFuncs.h"

//typedef void (processingFuncPtr*)(uint32_t sz, uint8_t *bPtr);

const uint32_t printEvery = 1000;

// define functions here for various forms of processing of the data coming from the boards

void serialPrintUint32(uint32_t &v){
  Serial.println(v);
}

boolean isFirstADCCID(timeValStruct_t &tvs){
  return ((tvs.aid == 1)&&(tvs.cid ==0));
}

void serialPrintTVS(timeValStruct_t &tvs){
  if(isFirstADCCID(tvs)){
    Serial.println("-------------------");
  }
  Serial.print("ADC_ID    : ");
  Serial.println(tvs.aid,DEC);    
  Serial.print("CID_ID    : ");  
  Serial.println(tvs.cid,DEC);
  Serial.print("Timestamp : ");  
  Serial.println(tvs.t,DEC);
  Serial.print("Value     : ");  
  Serial.println(tvs.v,DEC);
}

