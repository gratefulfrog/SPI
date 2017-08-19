#include "utilities.h"

//typedef void (processingFuncPtr*)(uint32_t sz, uint8_t *bPtr);

const uint32_t printEvery = 1000;

// define functions here for various forms of processing of the data coming from the boards

void serialPrintUint32(uint32_t &v){
  Serial.println(v);
}

bool isFirstADCCID(const uint8_t &aid,const uint8_t &cid){
  return ((aid == 1)&&(cid ==0));
}

bool isValid(const timeValStruct_t &tvs){
  static uint32_t lastTimeStamp= tvs.t;
  const static timeStamp_t  maxOverFlowTimeStamp = 100000;
  uint8_t aid,cid;
  decode(tvs.aidcid,aid,cid);
  bool  ok = true;
  if (aid !=1){
    //cout << "  bad ADC ID: " << (int)aid  << endl;
    Serial.print("  bad ADC ID: ");
    Serial.println((int)aid);
    ok = false;
  }
  else if (cid >7){
    //cout << "  bad Channel ID: " << (int)cid  << endl;
    Serial.print("  bad Channel ID: ");
    Serial.println((int)cid);
    ok = false;
  }
  else if ((tvs.t < lastTimeStamp) && (tvs.t > maxOverFlowTimeStamp)){
    //cout << "  bad TimeStamp: " << tvs.t  << endl;
    Serial.print("  bad TimeStamp: ");
    Serial.print(tvs.t);
    Serial.print("  last TimeStamp: ");
    Serial.print(lastTimeStamp);
    Serial.print("  difference: ");
    Serial.println(lastTimeStamp-tvs.t);
    ok = false;
  }
  else if (((cid % 2) == 0) && (tvs.v >0.1)){
    //cout << "  bad value for channel : " << (int)cid << " value : " << tvs.v << endl;
    Serial.print("  bad value for channel : ");
    Serial.print((int)cid);
    Serial.print(" value : ");
    Serial.println(tvs.v);
    ok = false;
  }
  else if ((cid % 2) && ((tvs.v < 2 ) || (tvs.v >3.5))){
    //cout << "  bad value for channel : " << (int)cid << " value : " << tvs.v << endl;
    Serial.print("  bad value for channel : ");
    Serial.print((int)cid);
    Serial.print(" value : ");
    Serial.println(tvs.v);
    ok = false;
  }
  else {
    lastTimeStamp= tvs.t;
  }
  return ok;
}

void serialPrintTVS(const timeValStruct_t &tvs, uint32_t &badCount){
  uint8_t aid, 
          cid;
  decode(tvs.aidcid,aid,cid);
  badCount = isValid(tvs) ? badCount : badCount +1;
  //delay(1);
  return;
  
  
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
