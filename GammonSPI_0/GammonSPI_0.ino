// Written by Nick Gammon
// January 2011

#include <SPI.h>

/* SPILIB uses the following PIN definitions
 * CLK   = 13
 * MIS0  = 12
 * MOSI  = 11 
 * SS    = 10
 */

const int ledPin = 13;

void flash(){
  while(true){
      digitalWrite(ledPin,LOW);
      delay(100);
      digitalWrite(ledPin,HIGH);
      delay(100);
  }
}


void setup (void){
  pinMode(ledPin,OUTPUT);
  flash();
  SPI.begin ();
  // This call:
  // Puts SCK, MOSI, SS pins into output mode
  // also puts SCK, MOSI into LOW state, and SS into HIGH state.
  // Then put SPI hardware into Master mode and turn SPI on
  
}


void loop (void){
  char c;
  
  // enable Slave Select
  digitalWrite(SS, LOW);    // SS is pin 10
  
  // send test string
  for (const char * p = "abc" ; c = *p; p++)
    SPI.transfer (c);
     
  // disable Slave Select
   digitalWrite(SS, HIGH);
 
  delay(200);
}

