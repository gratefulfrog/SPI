#include "adcMgr.h"
#include "ad7689.h"
#include "adxl345.h"
#include "kx122.h"

ADCMgr::ADCMgr(uint8_t nbChan) : nbChannels(nbChan){}              

void YADCMgr::doSelfTest() const {
  adc->selftest();
}

YSPI* YADCMgr::hwInit(uint8_t SSpin) const {
  // step 1: YSPI instantiation
  YSPI* y = new HWSPI(SSpin,F_CPU >= MAX_FREQ ? MAX_FREQ : F_CPU, MSBFIRST, SPI_MODE0); // HW SPI
  if(y){
    #ifdef DEBUG
      SerialUSB.println(String("HWSPI instance ") + String(adcID) + String(" created!"));
    #endif
  }
  else{
    #ifdef DEBUG
      SerialUSB.println(String("HWSPI instance ") + String(adcID) +  String(" failed!"));
    #endif
    while(1);
  }
  return y;
}

float  YADCMgr::getValue(uint8_t channel) const{
  return adc->acquireChannel(channel);
}

YADCMgr::YADCMgr(uint8_t adcID, uint8_t nbChan, uint8_t SSPin) : ADCMgr(nbChan){    
  YSPI *yspi;

  if (adcID<3){ // use spi
    // step 1 : yspi instance creation
    yspi = hwInit(SSPin); 
  }
  if (adcID <2){ // its an AD7689
    // step 2: ADC instantiation
    adc = new AD7689(yspi, nbChannels);
  }
  else if (adcID ==2){
    adc = new YADXL345(yspi);
  }
  else { // its a KX122
     adc = new YKX122(YKX122::addrVec[adcID-3]);
  }
  #ifdef DEBUG
    SerialUSB.println(String("adc truth value : ") + String(adc ? "true" : "false"));
  #endif
  
  if(adc){
    #ifdef DEBUG
      SerialUSB.println(String("ADC instance ") + String(adcID) + String(" created!"));
    #endif
  }
  else{
    #ifdef DEBUG
      SerialUSB.println(String("ADC instance ") + String(adcID) +   String(" creation failed!"));
    #endif
    while(1);
  }  // step 3: AD7689 self-test
  doSelfTest();
}

