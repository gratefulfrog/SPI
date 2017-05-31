#include "app.h"


void BobTestApp::testSetup() const{
  pinMode(hbPin,OUTPUT);
  pinMode(truePin,OUTPUT);
  pinMode(falsePin,OUTPUT);
  pinMode(idPin,OUTPUT);
  pinMode(yspiOnPin,INPUT);
}

void BobTestApp::heartBeat() const{
  digitalWrite(hbPin, !digitalRead(hbPin));
}

void BobTestApp::flash(boolean tf) const{
  int pin = tf ? truePin :falsePin;
  for(int i =0; i<nbInfoFlashes;i++){
      digitalWrite(pin,HIGH);
      delay(onTime);
      digitalWrite(pin,LOW);
      delay(tf ? trueOffTime : falseOffTime);
  }
}

void BobTestApp::doSelfTest(AD7689 *adc) const{
  if (adc->selftest()){
    (!usingUSARTSPI && useSerial && Serial.println("AD7689 connected and ready"));
    flash(true);
  } 
  else {
    (!usingUSARTSPI && useSerial && Serial.println("Error: AD7689  self Test Failed!"));
    while (1){
      flash(false);
    }    
  }
}

void BobTestApp::flashInfo(int n, bool once = false) const {
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

YSPI* BobTestApp::usartInit(uint8_t id) const{
  // step 1: YSPI instantiation
  flashInfo(1);
  delay(1000);
  YSPI   *yyy = new USARTSPI(id);  // UART SPI on uart 0
  flash(yyy);
  delay(1000);
  // step 2: AD7689 instantiation
  flashInfo(2);
  return yyy; 
}

YSPI* BobTestApp::hwInit() const{
  if (useSerial){
    Serial.begin(115200);
    while(!Serial);
    Serial.println("Let the test begin!"); 
    if (showHWSettings){
      Serial.println (String("HW SPI Frequency:\t") + String(F_CPU >= MAX_FREQ ? MAX_FREQ : F_CPU));  // 16 000 000 == 16MHz
    }
  }
  // step 1: YSPI instantiation
  flashInfo(1);
  delay(1000);
  YSPI   *yyy = new HWSPI(AD7689_SS_pin,F_CPU >= MAX_FREQ ? MAX_FREQ : F_CPU, MSBFIRST, SPI_MODE0); // HW SPI
  flash(yyy);
  (useSerial && Serial.println("HWSPI instance created!"));  
  return yyy;
}

boolean BobTestApp::checkChannelReading(int chan, float reading) const{
  float correctVal = chan %2 ? 3.3 : 0.0;
  return (abs(reading-correctVal)< epsilon);
}

void BobTestApp::checkAndTell(uint8_t adcID, uint8_t ch) const {
  uint32_t timeStamp = 0;    // updated by reads to the adc
  float reading      = adcVec[adcID]->acquireChannel(ch, &timeStamp); 
  
  if (!usingUSARTSPI && useSerial){
    Serial.print(ch==0 ? String("\nADC :\t") + String(adcID) + String("\n") :"");
    Serial.print("AD7689 voltage input "+ String(ch_cnt) + String(" :\t"));
    Serial.print(reading);
    Serial.print(String("\t") + String(checkChannelReading(ch,reading) ? "TRUE" : "FALSE"));
    Serial.println(String("\t") + String(timeStamp));
  }

  digitalWrite(idPin,ch_cnt);
  flashInfo(ch, true);
  flash(checkChannelReading(ch,reading));
}

BobTestApp::BobTestApp(uint8_t firstUart) : App(firstUart){
  adcVec =  new AD7689*[USARTSPI::nbUARTS];
  testSetup();
  usingUSARTSPI = digitalRead(yspiOnPin);

  instantiateADCs();
}

void BobTestApp::runLoop() {
  if (digitalRead(talkPin)){
    heartBeat();
    checkAndTell(currentADC, ch_cnt);
  }
  else{
    adcVec[currentADC]->acquireChannel(ch_cnt, NULL);
  }
  ch_cnt = (ch_cnt + 1) % nbChannels;
  if (!ch_cnt && usingUSARTSPI){
    currentADC = ((currentADC +1 %USARTSPI::nbUARTS) ? (currentADC +1 %USARTSPI::nbUARTS) : usartID0);
  }
  delay(250);
}

