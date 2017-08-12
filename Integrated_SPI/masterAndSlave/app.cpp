#include "app.h"

App::App(){
  Serial.begin(115200);
}

void App::printSendCount() const{
  static int sendCount = 0;
  String s = "Send: " + String(sendCount++) + "\n";
  Serial.print(s);
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

void App::printReply(unsigned long &v, boolean isTime){
  /*
  if (!consoleInput){
    return;
  }
  consoleInput = false;
  */
  if (isTime){
    Serial.print("Slave time: ");
  }
  else{
    Serial.print("Board ID: ");
  }
  Serial.println(v);
}

void App::printReply(timeValStruct_t &tvs){
  
  if (consoleInput && (tvs.aid != ADCMgr::nullADCID)){
    ADCMgr::serialPrintTVS(tvs);
    consoleInput = false; 
  }
  
  if (tvs.aid != ADCMgr::nullADCID){
    ADCMgr::serialPrintTVS(tvs);
    //delay(100);
  }
  
}


