/* HW SPI LIB uses the following PIN definitions
 * CLK   = 13 GREEN
 * MIS0  = 12 ORANGE
 * MOSI  = 11 BLUE
 * SS    = 10 YELLOW
 */

 /* YSPI pins (USART)
  D0 MISO ORANGE (Rx)
  D1 MOSI BLUE (Tx)
  D4 SCK  GREEN  (clock)
  D5 SS   YELLOW (slave select)  <-- this can be changed
  */

/* OTHER PINS
 *  5V powers the AD7689
 *  GND is also requried ont he AD7689
 *  
 *  ICSP HEADER: 
 *  pins 5 & 6 MUST BE connected together to prevent the ATmega16U from pulling MISO HIGH in USART mode!
 *  - these are the GND and REST pins.
 *  
 *  HeartBeat LED 9
 *  ID LED        8
 *  True PIN      7
 *  Fasle PIN     6
 *  use YSPI PIN  2  HIGH means YES, use YSPI, LOW means NO use HW SPI  
 */

#include <Arduino.h>
#include "ad7689.h"
#include "yspi.h"

const boolean useSerial      = false,
              showHWSettings = useSerial;

const uint8_t AD7689_SS_pin = 10,
              hbPin         = 9,
              idPin         = 8,
              truePin       = 7,
              falsePin      = 6,
              yspiOnPin     = 2,
              onTime        = 200,
              falseOffTime  = 600,
              trueOffTime   = onTime,
              nbInfoFlashes = 5,
              nbChannels    = 2;

const float chValues[] = { 0, 3.3},
            epsilon = 0.03;

AD7689 *adc;
YMSPI  *yyy;
uint8_t ch_cnt     = 0; // channel counter
boolean usingYSPI  = true;  // value changed during setup based on reading of yspiOnPin
uint32_t timeStamp = 0;    // updated by reads to the adc

void testSetup(){
  pinMode(hbPin,OUTPUT);
  pinMode(truePin,OUTPUT);
  pinMode(falsePin,OUTPUT);
  pinMode(idPin,OUTPUT);
  pinMode(yspiOnPin,INPUT);
}

void heartBeat(){
  static boolean on = false;
  on = ! on;
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
  
  // AD7689 connected through SPI with SS specified in constructor
  // use default settings (8 channels, unipolar, referenced to 4.096V internal bandga)
  
  if (usingYSPI){
    // step 1: YMSPI instantiation
    flashInfo(1);
    delay(1000);
    yyy = new YMSPI(1);
    flash(yyy);
    delay(1000);
    // step 2: AD7689 instantiation
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
      if (showHWSettings){
        Serial.println (String("Freq:\t") + String(F_CPU >= MAX_FREQ ? MAX_FREQ : F_CPU));  // 16 000 000 == 16MHz
      }
    }
    // step 2: AD7689 instantiation
    flashInfo(2);
    delay(1000);
    adc = new AD7689(AD7689_SS_pin,nbChannels);
    flash(adc);
    delay(1000);
  }
  // step 3: AD7689 self-test
  flashInfo(3);
  delay(1000);
  doSelfTest();
}

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

