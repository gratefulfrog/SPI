#include "utilities.h"

using namespace std;

/*******************************/
// some arduino compatible stuff
/*******************************/

void  delay(time_t millisecs){
  delayFull(millisecs/1000.0,0);
}

void  delayMicroseconds(long microsecs){
  struct timespec delayTime = {0,microsecs*1000};
  nanosleep(&delayTime,NULL);
}

void delaySeconds(time_t secs){
  struct timespec delayTime = {secs,0};
  nanosleep(&delayTime,NULL);
}

void delayFull(time_t secs, long micros){
  struct timespec delayTime = {secs,micros};
  nanosleep(&delayTime,NULL);
}

// define functions here for various forms of processing of the data coming from the boards

void serialPrintUint32(const uint32_t &v){
  cout << v << endl;
}

bool isFirstADCCID(const uint8_t &aid,const uint8_t &cid){
  return ((aid == 1)&&(cid ==0));
}

void serialPrintTVS(const timeValStruct_t &tvs){
  uint8_t aid, 
          cid;
  decode(tvs.aidcid,aid,cid);
  if(isFirstADCCID(aid,cid)){
    cout << "-------------------" << endl;
  }
  cout << "ADC_ID     : " << (unsigned)aid   << endl;
  cout << "CHANNEL_ID : " << (unsigned)cid   << endl;
  cout << "Timestamp  : " << tvs.t << endl;
  cout << "Value      : " << tvs.v << endl;
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

/*
void bid2Disk(FileMgr * fm, boardID &bid){
  fm->setBID(bid);
}

void tid2Disk(FileMgr * fm, timeStamp_t){
  fm->setTID();
}

void tvs2Disk(FileMgr * fm, timeValStruct_t &tvs){
  fm->addTVS(tvs);
}
*/
