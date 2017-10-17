#include "utilities.h"

//typedef void (processingFuncPtr*)(uint32_t sz, uint8_t *bPtr);

const uint32_t printEvery = 1000;

// define functions here for various forms of processing of the data coming from the boards

void serialPrintUint32(uint32_t &v){
  Serial.println(v);
}

boolean isFirstADCCID(const uint8_t &aid,const uint8_t &cid){
  return ((aid == 1)&&(cid ==0));
}

void serialPrintTVS(timeValStruct_t &tvs){
  uint8_t aid, 
          cid;
  decode(tvs.aidcid,aid,cid);
  if(isFirstADCCID(aid,cid)){
    Serial.println("-------------------");
  }
  Serial.print("ADC_ID     : ");
  Serial.println(aid,DEC);    
  Serial.print("CHANNEL_ID : ");  
  Serial.println(cid,DEC);
  Serial.print("Timestamp  : ");  
  Serial.println(tvs.t,DEC);
  Serial.print("Value      : ");  
  Serial.println(tvs.v,DEC);
}

// encodes bits LLLL, RRRR to LLLLRRRR , max value to encode is 15
void encode(uint8_t &coded, const uint8_t &left, const uint8_t &right){
  coded = ((left & 0xF) << 4 ) | (right & 0xF);
}

// decodes bits LLLLRRRR to LLLL, RRRR , max value to decode is 255
void decode(const uint8_t &coded, uint8_t &left, uint8_t &right){
  left = (coded >> 4) & 0xF;
  right = coded & 0xF;
}
