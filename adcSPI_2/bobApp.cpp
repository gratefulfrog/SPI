  #include "app.h"


void BobTestApp::testSetup(){
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

void BobTestApp::doSelfTest() const{
  if (adc->selftest()){
    (!usingUSARTSPI && useSerial && Serial.println("AD7689 Self-Test Passed!"));
    flash(true);
  } 
  else {
    (!usingUSARTSPI && useSerial && Serial.println("Error: AD7689  Self-Test Failed!"));
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

void BobTestApp::usartInit(){
  // step 1: YSPI instantiation
  flashInfo(1);
  delay(1000);
  yyy = new USARTSPI(0);  // UART SPI on uart 0
  flash(yyy);
  delay(1000);
  // step 2: AD7689 instantiation
  flashInfo(2);
  adc = new AD7689(yyy,nbChannels);
  flash(adc);
  delay(1000);  
}

void BobTestApp::hwInit(){
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
  yyy = new HWSPI(AD7689_SS_pin,F_CPU >= MAX_FREQ ? MAX_FREQ : F_CPU, MSBFIRST, SPI_MODE0); // HW SPI
  flash(yyy);
  (useSerial && Serial.println("HWSPI instance created!"));
  delay(1000);
  // step 2: AD7689 instantiation
  flashInfo(2);
  delay(1000);
  adc = new AD7689(yyy, nbChannels);
  flash(adc);
  (useSerial && Serial.println("AD7689 instance created!"));
  delay(1000);  
}

boolean BobTestApp::checkChannelReading(int chan, float reading) const{
  float correctVal = chan %2 ? 3.3 : 0.0;
  return (abs(reading-correctVal)< epsilon);
}

void BobTestApp::checkAndTell() const {
  float reading = adc->acquireChannel(ch_cnt, &timeStamp); 
  if (!usingUSARTSPI && useSerial){
    Serial.print(ch_cnt==0 ?"\n" :"");
    Serial.print("AD7689 voltage input "+ String(ch_cnt)+" :\t");
    Serial.print(reading);
    Serial.print(String("\t") + String(checkChannelReading(ch_cnt,reading) ? "TRUE" : "FALSE"));
    Serial.println(String("\t") + String(timeStamp));
  }

  digitalWrite(idPin,ch_cnt);
  flashInfo(ch_cnt, true);
  flash(checkChannelReading(ch_cnt,reading));
}


BobTestApp::BobTestApp() : App(){
  testSetup();
  usingUSARTSPI = digitalRead(yspiOnPin);
  
  // AD7689 connected through SPI with SS specified in constructor
  // use default settings (8 channels, unipolar, referenced to 4.096V internal bandga)
  
  if (usingUSARTSPI){
    usartInit();
  }
  else{  // HW SPI
    hwInit(); 
  }
  // step 3: AD7689 self-test
  flashInfo(3);
  delay(1000);
  doSelfTest();
}

void BobTestApp::runLoop() {
  if (digitalRead(talkPin)){
    heartBeat();
    checkAndTell();
  }
  else{
    adc->acquireChannel(ch_cnt, &timeStamp);
  }
  ch_cnt = (ch_cnt + 1) % nbChannels;
  delay(250);
}

