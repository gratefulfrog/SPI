#include "app.h"


void YannickTestApp::doSelfTest(){
  if (adc->selftest()){
    (useSerial && Serial.println("AD7689 connected and ready"));
  } 
  else {
    (useSerial && Serial.println("Error: AD7689  self Test Failed!"));
    while (1);
  }
}

void YannickTestApp::usartInit(){
  // step 1: YSPI instantiation
  delay(1000);
  yyy = new USARTSPI(usartID);  // UART SPI on uart 0
  if(yyy){
    (useSerial && Serial.println("USARTSPI instance created!"));
  }
  else{
    (useSerial && Serial.println("USARTSPI instantiation failed!!"));
    while(1);
  }
}

void YannickTestApp::hwInit(){
  // step 1: YSPI instantiation
  delay(1000);
  yyy = new HWSPI(AD7689_SS_pin,F_CPU >= MAX_FREQ ? MAX_FREQ : F_CPU, MSBFIRST, SPI_MODE0); // HW SPI
  if(yyy){
    (useSerial && Serial.println("HWSPI instance created!"));
  }
  else{
    (useSerial && Serial.println("HWSPI instantiation failed!!"));
    while(1);
  }
}

boolean YannickTestApp::checkChannelReading(int chan, float reading){
  float correctVal = chan %2 ? 3.3 : 0.0;
  return (abs(reading-correctVal)< epsilon);
}

void YannickTestApp::checkAndTell(){
  float reading = adc->acquireChannel(ch_cnt, &timeStamp); 
  if (useSerial){
    Serial.print(ch_cnt==0 ?"\n" :"");
    Serial.print("AD7689 voltage input "+ String(ch_cnt)+" :");
    Serial.print(reading);
    Serial.print(String("\t") + String(checkChannelReading(ch_cnt,reading) ? "TRUE" : "FALSE"));
    Serial.println(String("\t") + String(timeStamp));
  }
}

YannickTestApp::YannickTestApp(){    
  if (useSerial){
    Serial.begin(115200);
    while(!Serial);
    Serial.println("Let the test begin!"); 
    if (showHWSettings){
      Serial.println (String("HW SPI Frequency:\t") + String(F_CPU >= MAX_FREQ ? MAX_FREQ : F_CPU));  // 16 000 000 == 16MHz
    }
  }
  
  if (usingUSARTSPI){
    usartInit();
  }
  else{  // HW SPI
    hwInit(); 
  }
  // step 2: AD7689 instantiation
  delay(1000); 
  adc = new AD7689(yyy, nbChannels);
  if(adc){
    (useSerial && Serial.println("AD7689 instance created!"));
  }
  else{
    (useSerial && Serial.println("AD7689 instantiation failed!!"));
    while(1);
  } 
  // step 3: AD7689 self-test
  delay(1000);
  doSelfTest();
}

void YannickTestApp::runLoop(){
  if (talk){  
  checkAndTell();
  }
  else{
    adc->acquireChannel(ch_cnt, &timeStamp);
  }
  ch_cnt = (ch_cnt + 1) % nbChannels;
  delay(250);
}

