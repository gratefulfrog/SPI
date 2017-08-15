#include "app.h"

using namespace std;

const struct timespec App::slaveProcessingTime = {slaveDelay,0};

App::App():channel(APP_SPI_CHANNEL){}

void App::outgoingMsg(char* buf) const{
  // puts a char[] into the arg buf,

  static int sendCount = 0;
  sprintf(buf, "Send: %d\n", sendCount++);
}

void App::transferAndWait (unsigned char &inOut) const{
  wiringPiSPIDataRW(channel,&inOut, 1);
  nanosleep(&pauseStruct,NULL);
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
  if (tvs.aidcid != ADCMgr::nullADCID){
    (*pFuncPtrTVS)(tvs);
  }
}


