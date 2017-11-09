#include "adcMgr.h"
#include "ad7689.h"
#include "adxl345.h"
#include "kx122.h"

ADCMgr::ADCMgr(uint8_t id, uint8_t nbChan) : adcID(id), nbChannels(nbChan){}              

void YADCMgr::doSelfTest() const {
  //Serial.println("call to doSelfTest");
  
  if (adc->selftest()){
    //Serial.println(String("ADC instance ") + String(adcID) +String(" Self-Test Passed!"));
  }
  else{
    Serial.println(String("ADC instance ") + String(adcID) +String(" Self-Test FAILED!"));
    while(1);
  }
}

YSPI* YADCMgr::usartInit() const {
  // step 1: YSPI instantiation
  YSPI* y = new USARTSPI(adcID);  // UART SPI on uart 0
  if(y){
    //Serial.println(String("USARTSPI instance ") + String(adcID) + String(" created!"));
  }
  else{
    Serial.println(String("USARTSPI instance ") + String(adcID) + String(" failed!!"));
    //while(1);
  }
  return y;
}

YSPI* YADCMgr::hwInit() const {
  // step 1: YSPI instantiation
  YSPI* y = new HWSPI(AD7689_SS_pin,F_CPU >= MAX_FREQ ? MAX_FREQ : F_CPU, MSBFIRST, SPI_MODE0); // HW SPI
  if(y){
    //Serial.println(String("HWSPI instance ") + String(adcID) + String(" created!"));
  }
  else{
    Serial.println(String("HWSPI instance ") + String(adcID) +  String(" failed!"));
    while(1);
  }
  return y;
}

float  YADCMgr::getValue(uint8_t channel) const{
  return adc->acquireChannel(channel);
}

YADCMgr::YADCMgr(uint8_t id, uint8_t nbChan) : ADCMgr(id,nbChan){    
  usingUSARTSPI  = true;
  YSPI *yspi;
  // step 1 : instance creation
  if (usingUSARTSPI){
    if (id<=3){
      yspi = usartInit();
    }
    else{
      yspi = NULL;
    }
  }
  else{  // HW SPI
    yspi = hwInit(); 
  }
  
  // step 2: ADC instantiation
  if(id <3) { // it's an AD7689
    adc = new AD7689(yspi, nbChannels);
  }
  else if (id ==3) {  // it's and adxl345
    adc = new YADXL345(yspi);
  }
  else{ // it's a KX122  // id = 4,5
    adc = new YKX122(YKX122::addrVec[id-4]);
  }
  
  if(adc){
    //Serial.println(String("ADC instance ") + String(adcID) + String(" created!"));
  }
  else{
    Serial.println(String("ADC instance ") + String(adcID) +   String(" creation failed!"));
    while(1);
  }  // step 3: AD7689 self-test
  doSelfTest();
}

