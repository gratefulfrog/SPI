/* HW SPI LIB uses the following PIN definitions
 * CLK   = 13 GREEN
 * MIS0  = 12 ORANGE
 * MOSI  = 11 BLUE
 * SS    = 10 YELLOW
 */

 /* YSPI pins (USART) on Arduino UNO
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
 *  TALK pin      3  HIGH means flash results of each ADC channel query, LOW means just do it.
 *  use YSPI PIN  2  HIGH means YES, use YSPI, LOW means NO use HW SPI  
 */

#include <Arduino.h>

#include "app.h"

const boolean Yannick = true;

App *app;
void setup(){
  if (Yannick){
    app = new YannickTestApp();
  }
  else{
    app = new BobTestApp();  
  }
}
void loop(){
  app->runLoop();
}

