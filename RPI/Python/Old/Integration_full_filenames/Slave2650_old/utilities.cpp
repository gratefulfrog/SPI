#include "utilities.h"

// encodes bits LLLL, RRRR to LLLLRRRR , max value to encode is 15
void encode(uint8_t &coded, const uint8_t &left, const uint8_t &right){
  coded = ((left & 0xF) << 4 ) | (right & 0xF);
}

// decodes bits LLLLRRRR to LLLL, RRRR , max value to decode is 255
void decode(const uint8_t &coded, uint8_t &left, uint8_t &right){
  left = (coded >> 4) & 0xF;
  right = coded & 0xF;
}

/*  obsolete code
bool isValid(const timeValStruct_t &tvs){
  static timeStamp_t lastTimeStamp= tvs.t;
  const static timeStamp_t  maxOverFlowTimeStamp = 500000;
  uint8_t aid,cid;
  decode(tvs.aidcid,aid,cid);
  bool aidOK = (aid == 1),
    cidOK =  (cid < 8),
    tsOK =  ((tvs.t >= lastTimeStamp) || (tvs.t < maxOverFlowTimeStamp)),
    valOK = (tvs.v >=0) && (tvs.v < 3.5),
    ok = aidOK && cidOK && tsOK && valOK;
  if (!ok){
    Serial.print("\n\n****************************** TVS Rejected!");
    if (! aidOK){
      Serial.print("  bad ADC ID: ");
      Serial.print((int)aid);
    }
    if (! cidOK){
      Serial.print("  bad Channel ID: ");
      Serial.print((int)cid);
    }
    if (! tsOK){
      Serial.print("  bad TimeStamp: ");
      Serial.print(tvs.t) ;
    }
    if (! valOK){
      Serial.print("  bad Value: ");
      Serial.print(tvs.v) ;
    }
    Serial.print('\n');
  }
  return ok;
}


 oid serialPrintUint32(uint32_t &v){
  Serial.println(v);
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
  
 */
