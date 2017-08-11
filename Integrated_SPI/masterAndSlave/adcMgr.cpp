#include "adcMgr.h"



ADCMgr::ADCMgr(uint8_t id, Q<timeValStruct_t> *qq,Informer *inf) : adcID(id), 
                                                                   q(qq),
                                                                   informer(inf){}              

void ADCMgr::serialPrintTVS(timeValStruct_t &tvs){
  Serial.print("ADC_ID    : ");
  Serial.println(tvs.aid);
  Serial.print("CID_ID    : ");  
  Serial.println(tvs.cid);
  Serial.print("Timestamp : ");  
  Serial.println(tvs.t);
  Serial.print("Value     : ");  
  Serial.println(tvs.v);
}

void YADCMgr::doSelfTest() const {
  if (adc->selftest()){
    informer->informOnce(3);
  }
  else{
    informer->informForever(3);
  }
}

YSPI* YADCMgr::usartInit() const {
  // step 1: YSPI instantiation
  delay(1000);
  YSPI* y = new USARTSPI(adcID);  // UART SPI on uart 0
  if(y){
    informer->informOnce(1);
  }
  else{
    informer->informForever(1);
  }
  return y;
}

YSPI* YADCMgr::hwInit() const {
  // step 1: YSPI instantiation
  delay(1000);
  YSPI* y = new HWSPI(AD7689_SS_pin,F_CPU >= MAX_FREQ ? MAX_FREQ : F_CPU, MSBFIRST, SPI_MODE0); // HW SPI
  if(y){
    informer->informOnce(1);
  }
  else{
    informer->informForever(1);
  }
  return y;
}
/*
const float YADCMgr::correctChannelReadingVec[][nbChannels] =  {{0.0, 3.3 , 0.0, 3.3, 0.0, 3.3, 0.0, 3.3}, 
                                                                       {0.0, 3.3 , 0.0, 3.3, 0.0, 3.3, 0.0, 3.3}, 
                                                                       {0.0, 3.3 , 0.0, 3.3, 0.0, 3.3, 0.0, 3.3}, 
                                                                       {0.0, 3.3 , 0.0, 3.3, 0.0, 3.3, 0.0, 3.3}};
                                                               
boolean YADCMgr::checkChannelReading(uint8_t chan, float reading) const{
  return (abs(reading-correctChannelReadingVec[adcID][chan])< epsilon);
}
*/
void YADCMgr::checkAndPush(uint8_t channel) const{
  //uint32_t timeStamp = 0;
  timeValStruct_t *tvs = new timeValStruct_t;
  tvs->v = adc->acquireChannel(channel, &tvs->t);
  noInterrupts();
  q->push(tvs);
  interrupts();
}

YADCMgr::YADCMgr(uint8_t id, Q<timeValStruct_t> *q, Informer *inf) : ADCMgr(id,q,inf){    
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
  delay(1000); 
  adc = new AD7689(yspi, nbChannels);
  if(adc){
    informer->informOnce(2);
  }
  else{
    informer->informForever(2);
  }  // step 3: AD7689 self-test
  delay(1000);
  doSelfTest();
}

void YADCMgr::runLoop() const{
  //println(String("\nADC : ") + String(adcID));
  for (uint8_t channel = 0; channel< nbChannels; channel++){
    checkAndPush(channel);  
  }
}

