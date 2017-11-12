#include "adcMgr.h"
#include "ad7689.h"
#include "adxl345.h"
//#include "kx122.h"

ADCMgr::ADCMgr(uint8_t id, uint8_t nbChan) : adcID(id), nbChannels(nbChan){}              

void YADCMgr::doSelfTest() const {
  adc->selftest();
}

YSPI* YADCMgr::usartInit() const {}
/*  // step 1: YSPI instantiation
  YSPI* y = new USARTSPI(adcID);  // UART SPI on uart 0
  if(y){
    //SerialUSB.println(String("USARTSPI instance ") + String(adcID) + String(" created!"));
  }
  else{
    SerialUSB.println(String("USARTSPI instance ") + String(adcID) + String(" failed!!"));
    //while(1);
  }
  return y;
}
*/
YSPI* YADCMgr::hwInit() const {
  // step 1: YSPI instantiation
  YSPI* y = new HWSPI(AD7689_SS_pin,F_CPU >= MAX_FREQ ? MAX_FREQ : F_CPU, MSBFIRST, SPI_MODE0); // HW SPI
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

YADCMgr::YADCMgr(uint8_t id, uint8_t nbChan) : ADCMgr(id,nbChan){    
  usingUSARTSPI  = false;
  YSPI *yspi;
  // step 1 : instance creation
  yspi = hwInit(); 
  
  // step 2: ADC instantiation
  adc = new AD7689(yspi, nbChannels);

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

