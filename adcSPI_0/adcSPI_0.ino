/* SPILIB uses the following PIN definitions
 * CLK   = 13 GREEN
 * MIS0  = 12 ORANGE
 * MOSI  = 11 BLUE
 * SS    = 10 YELLOW
 */

 /* YSPI pins
  D0 MISO ORANGE (Rx)
  D1 MOSI BLUE (Tx)
  D4 SCK  GREEN  (clock)
  D5 SS   YELLOW (slave select)  <-- this can be changed
  */

/* OTHER PINS
 *  5V powers the AD7689
 *  GND is also requried ont he AD7689
 *  
 *  HeartBeat LED 9
 *  ID LED        8
 *  True PIN      7
 *  Fasle PIN     6
 *  use YSPI PIN  2  HIGH means YES, use YSPI, LOW means NO use HW SPI  
 */


// this example illustrates how all ADC channels can be read with default settings:
// unipolar configuration, 8 channels, 4.096V internal positive voltage reference, negative referenced to ground

#include <Arduino.h>

#include "ad7689.h"
#include "yspi.h"

const boolean useSerial = false;  
boolean usingYSPI = true;

const int hbPin         = 9,
          idPin         = 8,
          truePin       = 7,
          falsePin      = 6,
          yspiOnPin     = 2,
          onTime        = 200,
          falseOffTime  = 600,
          trueOffTime   = onTime,
          nbInfoFlashes = 5;

void testSetup(){
  pinMode(hbPin,OUTPUT);
  pinMode(truePin,OUTPUT);
  pinMode(falsePin,OUTPUT);
  pinMode(idPin,OUTPUT);
  pinMode(yspiOnPin,INPUT);
}

void heartBeat(){
  static int now = millis();
  static boolean on = true;
  //if (millis()-now>=hbTime){
    on = ! on;
    now= millis();
  //}
  digitalWrite(hbPin,on ? HIGH : LOW);
}

void flash(boolean tf){
  int pin = tf ? truePin :falsePin;
  for(int i =0; i<nbInfoFlashes;i++){
      digitalWrite(pin,HIGH);
      delay(onTime);
      digitalWrite(pin,LOW);
      delay(tf ? trueOffTime : falseOffTime);
  }
}

AD7689 *adc;
YMSPI *yyy;
const uint8_t AD7689_SS_pin = 10;
uint8_t ch_cnt = 0; // channel counter
const uint8_t nbChannels = 2;


void doSelfTest(){
  if (adc->selftest()){
    (!usingYSPI && useSerial && Serial.println("AD7689 connected and ready"));
    flash(true);
  } 
  else {
    (!usingYSPI && useSerial && Serial.println("Error: AD7689  self Test Failed!"));
    while (1){
      flash(false);
    }
    
  }
}
void flashInfo(int n){
  digitalWrite(idPin,LOW);
  for (int j = 0; j<3;j++){
    for (int i = 0; i<n;i++){
      digitalWrite(idPin,HIGH);
      delay(200);
      digitalWrite(idPin,LOW);
      delay(200);
    }
    delay(600);
  }
}
void setup() {  
  testSetup();
  usingYSPI = digitalRead(yspiOnPin);
  pinMode(0,INPUT);
  
  // AD7689 connected through SPI with SS specified in constructor
  // use default settings (8 channels, unipolar, referenced to 4.096V internal bandga)
  
  if (usingYSPI){
    flashInfo(1);
    delay(1000);
    yyy = new YMSPI(1);
    flash(yyy);
    delay(1000);
    //while(1); 
    flashInfo(2);
    adc = new AD7689(yyy,nbChannels);
    flash(adc);
    delay(1000);
  }
  else{
    if (useSerial){
      Serial.begin(115200);
      while(!Serial);
      Serial.println("let the test begin!"); 
      Serial.println("adc instance created");
    }
    flashInfo(1);
    delay(1000);
    adc = new AD7689(AD7689_SS_pin,nbChannels);
    flash(adc);
    delay(1000);
  }
  flashInfo(3);
  delay(1000);
  doSelfTest();
}

uint32_t timeStamp =0;

const float chValues[] = { 0, 3.3},
            epsilon = 0.03;

boolean checkChannelReading(int chan, float reading){
  return (abs(reading-chValues[chan])< epsilon);
}

void checkAndTell(){
  float reading = adc->acquireChannel(ch_cnt, &timeStamp); 
  if (!usingYSPI && useSerial){
    Serial.print(ch_cnt==0 ?"\n" :"");
    Serial.print("AD7689 voltage input "+ String(ch_cnt)+" :");
    Serial.print(reading);
    Serial.print(String("\t") + String(checkChannelReading(ch_cnt,reading) ? "TRUE" : "FALSE"));
    Serial.println(String("\t") + String(timeStamp));
  }

  digitalWrite(idPin,ch_cnt);
  flash(checkChannelReading(ch_cnt,reading));
}

void loop() {
  heartBeat();
  
  checkAndTell();
  ch_cnt = (ch_cnt + 1) % nbChannels;
  delay(250);
  
}

