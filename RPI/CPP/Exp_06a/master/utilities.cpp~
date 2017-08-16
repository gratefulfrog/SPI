#include "utilities.h"

using namespace std;

/*******************************/
// some arduino compatible stuff
/*******************************/

void  delay(time_t millisecs){
  delayMicroseconds(millisecs*1000);
}

void  delayMicroseconds(long microsecs){
  struct timespec delayTime = {0,microsecs*1000};
  nanosleep(&delayTime,NULL);
}

// define functions here for various forms of processing of the data coming from the boards

void serialPrintUint32(uint32_t &v){
  cout << v << endl;
}

bool isFirstADCCID(const uint8_t &aid,const uint8_t &cid){
  return ((aid == 1)&&(cid ==0));
}

void serialPrintTVS(timeValStruct_t &tvs){
  uint8_t aid, 
          cid;
  decode(tvs.aidcid,aid,cid);
  if(isFirstADCCID(aid,cid)){
    cout << "-------------------" << endl;
  }
  cout << "ADC_ID     : " << aid   << endl;
  cout << "CHANNEL_ID : " << cid   << endl;
  cout << "Timestamp  : " << tvs.t << endl;
  cout << "Value      : " << tvs.v << endl;
  /*
  Serial.print("ADC_ID     : ");
  Serial.println(aid,DEC);    
  Serial.print("CHANNEL_ID : ");  
  Serial.println(cid,DEC);
  Serial.print("Timestamp  : ");  
  Serial.println(tvs.t,DEC);
  Serial.print("Value      : ");  
  Serial.println(tvs.v,DEC);
  */
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
