#include "adcMgr.h"

ADCMgr::ADCMgr(uint8_t id, uint8_t nbChan) : adcID(id), nbChannels(nbChan){}              

void YADCMgr::doSelfTest() const {
  //Serial.println("call to doSelfTest");
  if (adc->selftest()){
    Serial.println(String("AD7689 instance ") + String(adcID) +String(" Self-Test Passed!"));
  }
  else{
    Serial.println(String("AD7689 instance ") + String(adcID) +String(" Self-Test FAILED!"));
    while(1);
  }
}

YSPI* YADCMgr::usartInit() const {
  // step 1: YSPI instantiation
  //delay(1000);
  YSPI* y = new USARTSPI(adcID);  // UART SPI on uart 0
  if(y){
    Serial. println(String("USARTSPI instance ") + String(adcID) + String(" created!"));
  }
  else{
    Serial.println(String("USARTSPI instance ") + String(adcID) + String(" failed!!"));
    while(1);
  }
  return y;
}

YSPI* YADCMgr::hwInit() const {
  // step 1: YSPI instantiation
  //delay(1000);
  YSPI* y = new HWSPI(AD7689_SS_pin,F_CPU >= MAX_FREQ ? MAX_FREQ : F_CPU, MSBFIRST, SPI_MODE0); // HW SPI
  if(y){
    Serial.println(String("HWSPI instance ") + String(adcID) + String(" created!"));
  }
  else{
    Serial.println(String("HWSPI instance ") + String(adcID) +  String(" failed!"));
    while(1);
  }
  return y;
}

float  YADCMgr::getValue(uint8_t channel) const{
  return adc->acquireChannel(channel, NULL);
}

YADCMgr::YADCMgr(uint8_t id, uint8_t nbChan) : ADCMgr(id,nbChan){    
  usingUSARTSPI  = true;
  
  YSPI *yspi;
  // step 1 : instance creation
  if (usingUSARTSPI){
    yspi = usartInit();
  }
  else{  // HW SPI
    yspi = hwInit(); 
  }
  // step 2: AD7689 instantiation
  //delay(1000); 
  adc = new AD7689(yspi, nbChannels);
  if(adc){
    Serial.println(String("AD7689 instance ") + String(adcID) + String(" created!"));
  }
  else{
    Serial.println(String("AD7689 instance ") + String(adcID) +   String(" creation failed!"));
    while(1);
  }  // step 3: AD7689 self-test
  //delay(1000);
  doSelfTest();
}


