/** HW SPI LIB uses the following PIN definitions
 * CLK   = 13 GREEN
 * MIS0  = 12 ORANGE
 * MOSI  = 11 BLUE
 * SS    = 10 YELLOW
 */

 /** YSPI pins (USART) on Arduino UNO
  D0 MISO ORANGE (Rx)
  D1 MOSI BLUE (Tx)
  D4 SCK  GREEN  (clock)
  D5 SS   YELLOW (slave select)  <-- this can be changed
  */

/** OTHER PINS
 *  5V powers the AD7689
 *  GND is also requried ont he AD7689
 *  
 *  Arduino UNO ICSP HEADER: 
 *  pins 5 & 6 MUST BE connected together to prevent the ATmega16U from pulling MISO HIGH in USART mode!
 *  - these are the GND and REST pins.
 *  
 *  Iteaduino Mega XU2 ICSP HEADER: 
 *  pins 5 & 6 MUST BE connected together to prevent the ATmega16U from pulling MISO HIGH on USART0!
 *  - these are the GND and REST pins and are the ones farthest from the edge of the board with the USB Connector:
 *    
 *    USB     |
 *            |
 *    R x x   |
 *    G x x   |
 *    --------
 *  
 *  HeartBeat LED 9
 *  ID LED        8
 *  True PIN      7
 *  False PIN     6
 *  TALK pin      4  HIGH means flash results of each ADC channel query, LOW means just do it.
 *  use YSPI PIN  3  HIGH means YES, use YSPI, LOW means NO use HW SPI  
 */

#include <Arduino.h>

#include "app.h"

const boolean Yannick = true;   // set to true when using Yannick's harvesting platform
              
const uint8_t nbUsarts2Test              = 1,          // this is how many USARTs we will test, GET IT RIGHT!!
              Usarts2Test[nbUsarts2Test] = {1} ;       // this is the vector of USART id that will be tested!

const App  **appVec ;

/** is2Test returns true if the id in argument is to be tested, false otherwise
 *  @param id : the id to be tested for testing
 *  @return   : true if id should be tested, false otherwise
 */
boolean is2Test(uint8_t uID){
  for (uint8_t i=0;i< nbUsarts2Test;i++){
    if(Usarts2Test[i] == uID){
      return true;
    }
  }
  return false;
}
/** getApp returns a pointer to an appropriate App instance
 *  @param isYannick : if true will return a pointer to a YTestApp, otherwise a BTestApp
 *  @param id        : the id to pass to the App instance constructor
 *  @return          : pointer to the heap allocated App instance.
 */
App* getApp(boolean isYannick, uint8_t id){
  return (isYannick ?  static_cast<App*>(new YTestApp(id)) 
                    :  static_cast<App*>(new BTestApp(id)));
}

void setup(){
  appVec = new App*[USARTSPI::nbUARTS];
  
  for(uint8_t i=0;i<USARTSPI::nbUARTS;i++){
    if (is2Test(i)){  
        appVec[i] = getApp(Yannick,i);
        
      }
      else{ 
        appVec[i] = NULL;  // if not to be tested, we set this to NULL 
      }
  }
}
void loop(){
  for(uint8_t i=0;i<USARTSPI::nbUARTS;i++){
    if(appVec[i]){  // check to be sure the pointer is not NULL!!
      appVec[i]->runLoop();
    }
  }
}

