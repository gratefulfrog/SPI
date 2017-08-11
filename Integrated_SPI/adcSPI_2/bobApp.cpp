#include "app.h"

void BTestApp::print(String s) const{
  if(!usingUSARTSPI  && 
     digitalRead(talkPin)){
    App::print(s);
  }
}

void BTestApp::heartBeat() const{
  digitalWrite(hbPin, !digitalRead(hbPin));
}

void BTestApp::testSetup() const{
  pinMode(hbPin,OUTPUT);
  pinMode(truePin,OUTPUT);
  pinMode(falsePin,OUTPUT);
  pinMode(idPin,OUTPUT);
  pinMode(yspiOnPin,INPUT);
}

void BTestApp::flash(boolean tf) const{
  if(!digitalRead(talkPin)){
    return;
  }
  int pin = tf ? truePin :falsePin;
  for(int i =0; i<nbInfoFlashes;i++){
      digitalWrite(pin,HIGH);
      delay(onTime);
      digitalWrite(pin,LOW);
      delay(tf ? trueOffTime : falseOffTime);
  }
}

void BTestApp::doSelfTest() const{
  if (adc->selftest()){
   println(String("AD7689 instance ") + String(adcID) +String(" Self-Test Passed!"));
    flash(true);
  } 
  else {
    println(String("AD7689 instance ") + String(adcID) +String(" Self-Test FAILED!"));
    while (1){
      flash(false);
    }    
  }
}

void BTestApp::flashInfo(int n, bool once = false) const {
  if(!digitalRead(talkPin)){
    return;
  }
  
  const int nbReps = once ? 1 : 3;
  
  digitalWrite(idPin,LOW);
  for (int j = 0; j< nbReps;j++){
    for (int i = 0; i<n;i++){
      digitalWrite(idPin,HIGH);
      delay(200);
      digitalWrite(idPin,LOW);
      delay(200);
    }
    delay(600);
  }
}

YSPI* BTestApp::usartInit() const {
  // step 1: YSPI instantiation
  flashInfo(1);
  YSPI* y = new USARTSPI(adcID);  // on Arduino UNO we only have UART SPI on uart 0
  flash(y);
  while(!y);
  return y;
}

YSPI* BTestApp::hwInit() const {
  if (useSerial){
    Serial.begin(115200);
    while(!Serial);
    println("Let the test begin!"); 
    if (showHWSettings){
      println (String("HW SPI Frequency : ") + String(F_CPU >= MAX_FREQ ? MAX_FREQ : F_CPU));  // 16 000 000 == 16MHz
    }
  }
  // step 1: YSPI instantiation
  flashInfo(1);
  YSPI* y = new HWSPI(AD7689_SS_pin,F_CPU >= MAX_FREQ ? MAX_FREQ : F_CPU, MSBFIRST, SPI_MODE0); // HW SPI
  flash(y);
  if (y){
    println(String("HWSPI instance ") + String(adcID) + String(" created!"));
  }
  else{
    println(String("HWSPI instance ") + String(adcID) +  String(" failed!"));
    while(1);
  }

  return y;
}

boolean BTestApp::checkChannelReading(uint8_t chan, float reading) const{
  float correctVal = chan %2 ? 3.3 : 0.0;
  return (abs(reading-correctVal)< epsilon);
}

void BTestApp::checkAndTell(uint8_t channel) const {
  uint32_t timeStamp = 0;
  float reading = adc->acquireChannel(channel, &timeStamp); 
  print("Voltage input "+ String(channel)+" :\t");
  print(String(reading));
  print(String("\t") + String(checkChannelReading(channel,reading) ? "TRUE" : "FALSE"));
  println(String("\t") + String(timeStamp));

  digitalWrite(idPin,channel);
  flashInfo(channel, true);
  flash(checkChannelReading(channel,reading));
}

BTestApp::BTestApp(uint8_t id) : App(id){
  testSetup();
  usingUSARTSPI = digitalRead(yspiOnPin);
  
  YSPI *yspi;
  
  if (usingUSARTSPI){
    yspi = usartInit();
  }
  else{  // HW SPI
    yspi = hwInit(); 
  }
  
  // step 2: AD7689 instantiation
  flashInfo(2);
  adc = new AD7689(yspi, nbChannels);
  flash(adc);
  if (adc){
    println(String("AD7689 instance ") + String(adcID) + String(" created!"));
  }
  else{
    println(String("AD7689 instance ") + String(adcID) +   String(" creation failed!"));
    while(1);
  }
  
  // step 3: AD7689 self-test
  flashInfo(3);
  doSelfTest();
}

void BTestApp::runLoop() const {
  println(String("\nADC : ") + String(adcID) + String(" (HWSPI)"));
  for (uint8_t channel = 0; channel< nbChannels; channel++){
    if (digitalRead(talkPin)){
      heartBeat();
      checkAndTell(channel);
    }
    else{
      adc->acquireChannel(channel, NULL);
    }
    delay(250);
  }
}

