#include "app.h"

void YannickTestApp::print(String s) const{
  if(talk){
    App::print(s);
  }
}

void YannickTestApp::doSelfTest() const {
  if (adc->selftest()){
    println(String("AD7689 instance ") + String(adcID) +String(" Self-Test Passed!"));
  } 
  else {
    println(String("AD7689 instance ") + String(adcID) +String(" Self-Test FAILED!"));
    while (1);
  }
}

YSPI* YannickTestApp::usartInit() const {
  // step 1: YSPI instantiation
  delay(1000);
  YSPI* y = new USARTSPI(adcID);  // UART SPI on uart 0
  if(y){
    println(String("USARTSPI instance ") + String(adcID) + String(" created!"));
  }
  else{
    println(String("USARTSPI instance ") + String(adcID) + String(" failed!!")); 
    while(1);
  }
  return y;
}

YSPI* YannickTestApp::hwInit() const {
  // step 1: YSPI instantiation
  delay(1000);
  YSPI* y = new HWSPI(AD7689_SS_pin,F_CPU >= MAX_FREQ ? MAX_FREQ : F_CPU, MSBFIRST, SPI_MODE0); // HW SPI
  if (y){
    println(String("HWSPI instance ") + String(adcID) + String(" created!"));
  }
  else{
    println(String("HWSPI instance ") + String(adcID) +  String(" failed!"));
    while(1);
  }
  return y;
}

const float YannickTestApp::correctChannelReadingVec[][nbChannels] =  {{0.0, 3.3 , 0.0, 3.3, 0.0, 3.3, 0.0, 3.3}, 
                                                                       {0.0, 3.3 , 0.0, 3.3, 0.0, 3.3, 0.0, 3.3}, 
                                                                       {0.0, 3.3 , 0.0, 3.3, 0.0, 3.3, 0.0, 3.3}, 
                                                                       {0.0, 3.3 , 0.0, 3.3, 0.0, 3.3, 0.0, 3.3}};
                                                               
boolean YannickTestApp::checkChannelReading(uint8_t chan, float reading) const{
  return (abs(reading-correctChannelReadingVec[adcID][chan])< epsilon);
}

void YannickTestApp::checkAndTell(uint8_t channel) const{
  uint32_t timeStamp = 0;
  float reading = adc->acquireChannel(channel, &timeStamp); 
  print("AD7689 voltage input "+ String(channel)+" :\t");
  print(String(reading));
  print(String("\t") + String(checkChannelReading(channel,reading) ? "TRUE" : "FALSE"));
  println(String("\t") + String(timeStamp));
}

YannickTestApp::YannickTestApp(uint8_t id) : App(id){    
  usingUSARTSPI  = true;
  if (useSerial){
    Serial.begin(115200);
    while(!Serial);
    println("Let the test begin!"); 
    println(String("Using:\t") + String(usingUSARTSPI ? "USART" : "HW SPI"));
    if (showHWSettings){
      println (String("HW SPI Frequency:\t") + String(F_CPU >= MAX_FREQ ? MAX_FREQ : F_CPU));  // 16 000 000 == 16MHz
    }
  }
  YSPI *yspi;
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
    println(String("AD7689 instance ") + String(adcID) + String(" created!"));
  }
  else{
    println(String("AD7689 instance ") + String(adcID) +   String(" creation failed!"));
    while(1);
  } 
  // step 3: AD7689 self-test
  delay(1000);
  doSelfTest();
}

void YannickTestApp::runLoop() const{
  println(String("\nADC : ") + String(adcID));
  for (uint8_t channel = 0; channel< nbChannels; channel++){
    if (talk){  
      //heartBeat();
      checkAndTell(channel);
    }
    else{
      adc->acquireChannel(channel, NULL);
    }
    delay(250);
  }
}

