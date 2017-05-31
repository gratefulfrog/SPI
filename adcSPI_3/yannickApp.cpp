#include "app.h"


void YannickTestApp::doSelfTest(AD7689 *adc) const {
  if (adc->selftest()){
    (useSerial && Serial.println("AD7689 Self-Test Passed!"));
  } 
  else {
    (useSerial && Serial.println("Error!! AD7689  Self-Test FAILED!"));
    while (1);
  }
}

YSPI* YannickTestApp::usartInit(uint8_t id) const{
  // step 1: YSPI instantiation
  delay(1000);
  YSPI *yyy = new USARTSPI(id);  // UART SPI on uart 0
  if(yyy){
    (useSerial && Serial.println("USARTSPI instance :\t") + String(id) + String(" created!"));
  }
  else{
    (useSerial && Serial.println("USARTSPI instantiation :\t") + String(id) + String(" failed!!"));
    while(1);
  }
  return yyy;
}

YSPI* YannickTestApp::hwInit() const{
  // step 1: YSPI instantiation
  delay(1000);
  YSPI *yyy = new HWSPI(AD7689_SS_pin,F_CPU >= MAX_FREQ ? MAX_FREQ : F_CPU, MSBFIRST, SPI_MODE0); // HW SPI
  if(yyy){
    (useSerial && Serial.println("HWSPI instance created!"));
  }
  else{
    (useSerial && Serial.println("HWSPI instantiation failed!!"));
    while(1);
  }
  return yyy;
}

const float YannickTestApp::correctVec[][8] = {{0, 2.85, 4.1 ,4.1 ,4.1 ,4.1 ,4.1 ,4.1},
                                               {0, 2.85, 4.1 ,4.1 ,4.1 ,4.1 ,4.1 ,4.1},
                                               {0, 2.85, 4.1 ,4.1 ,4.1 ,4.1 ,4.1 ,4.1},
                                               {0, 2.85, 4.1 ,4.1 ,4.1 ,4.1 ,4.1 ,4.1}};

boolean YannickTestApp::checkChannelReading(uint8_t adcID, uint8_t chan, float reading) const{
  //float correctVal = chan %2 ? 3.3 : 0.0;
  //return (abs(reading-correctVal)< epsilon);
  return (abs(reading-correctVec[adcID][chan])< epsilon);
}

void YannickTestApp::checkAndTell(uint8_t adcID, uint8_t ch) const{
  uint32_t timeStamp = 0;    // updated by reads to the adc
  float reading = adcVec[adcID]->acquireChannel(ch_cnt, &timeStamp); 
  if (useSerial){
    Serial.print(ch==0 ? String("\nADC :\t") + String(adcID) + String("\n") : "");
    Serial.print("AD7689 voltage input "+ String(ch) + String(" :\t"));
    Serial.print(reading);
    Serial.print(String("\t") + String(checkChannelReading(adcID,ch,reading) ? "TRUE" : "FALSE"));
    Serial.println(String("\t") + String(timeStamp));
  }
}

YannickTestApp::YannickTestApp(const boolean *adcs2Test) : App(adcs2Test){
  adcVec =  new AD7689*[USARTSPI::nbUARTS]; 
  if (useSerial){
    Serial.begin(115200);
    while(!Serial);
    Serial.println("Let the test begin!"); 
    Serial.println(String("Using:\t") + String(usingUSARTSPI ? "USART" : "HW SPI"));
    if (!usingUSARTSPI && showHWSettings){
      Serial.println (String("HW SPI Frequency:\t") + String(F_CPU >= MAX_FREQ ? MAX_FREQ : F_CPU));  // 16 000 000 == 16MHz
    }
  }
  instantiateADCs();
}

void YannickTestApp::runLoop() {
  if (talk){  
  checkAndTell(currentADC, ch_cnt);
  }
  else{
    adcVec[currentADC]->acquireChannel(ch_cnt, NULL);
  }
  updateADCAndChannelCounters();
  delay(250);
}
