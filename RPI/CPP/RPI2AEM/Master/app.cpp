#include "app.h"

using namespace std;

const processingUint32FuncPtr  App::pFuncPtrUint32 = &serialPrintUint32;
const processingUintTVSFuncPtr App::pFuncPtrTVS    = &serialPrintTVS;

App::App(int chan, int sped, uint8_t nbADCChannels:)
  bidWriterFunc(&FileMgr::setBID),
  tidWriterFunc(&FileMgr::SetTID),
  tvsWriterFunc(&FileMgr::addTVS),{
  spi = new SPI(chan,sped);
  // XXXX NOT COMPLETE !!!!!!
  fm = new FileMgr(nbADCChannels);
  
}


/*
void App::outgoingMsg(char* buf) const{
  // puts a char[] into the arg buf,

  static int sendCount = 0;
  sprintf(buf, "Send: %d\n", sendCount++);
}
*/


uint8_t App::transferAndWait (const uint8_t what) const{
  uint8_t res = spi->transfer(what);
  delayMicroseconds(pauseBetweenSends);
  return res;
} 

void App::processReply(uint32_t v, bool isTime){
  if (isTime){
    cout << "Slave time: ";
    (fm->*tidWriterFunc)();
  }
  else{
    cout << "Board ID: ";
    (fm->*bidWriterFunc)(v);
  }
  (*pFuncPtrUint32)(v);
}

void App::processReply(timeValStruct_t &tvs){
  if (tvs.aidcid != nullADCID){
    (*pFuncPtrTVS)(tvs);
    (fm->*tvsWriterFunc)(tvs);
  }
}


