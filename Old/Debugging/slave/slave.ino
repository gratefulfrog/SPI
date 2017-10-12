/*
 * This is the DEBUGGING SLAVE VERSION !!!
 */


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
 *  GND is also requried on the AD7689
 */

/* USAGE:
 *  1. connect the master and slave boards via SPI
 *  2. power up the 2 boards
 *  3. open serial monitors to both board (needed in current version with Serial Output!
 *  3. reset the MEGA, and IMMEDIATLY hold down reset on the UNO
 *  4. when the MEGA montor shows "waiting for init call", release the rest button on the UNO;
 *  5. processing and comms begin !
 */


/*
 * This version is Master polls slave with commands  corrsponding to App::initChar, App::bidChar, App::acquireChar via SPI: 
 **  initChar: sets slave time to zero, clears q, then ready to go!
 **  bidChar: identify expects a GUID in repsonse 
 **  acquireChar: acquire expects a timeValStruct_t in respnse, ALWAYS, so even if no data is available a Null Response must be sent!
 * Slave repsonds apprpriately.
 * Slave will crash if Q over flows!
 * Slave needs to pause between reading sensors or the q will over flow since the Master cannot get the replies fast enough...
 * if no slave data is available at poll, the nullReturn is sent.
 * 
 * The variables below are needed to:
 * TUNE the TIMING : This is needed to avoid board Q overflow and catastrophic failures. It seems that the fastest we 
 *                 : do is an ADC channel reading around every 50 microseconds, but some channels seem to take 10x longer!,  
 * 
 * SPI_anything.h:  
 **  const int pauseBetweenSends : is the time in micro-seconds that is waited after an SPI.transfer or a digitalWrite(SS, LOW); 
 * app.h:
 **  App:slaveProcessingTime : is the time waited between each poll from the master to the slave, this should be zero! 
 *                           : it's ok if this is too high since a nullReturn value will be sent by the slave if no new data is available
 *                           : the nullREturn can be ignored by the master.
 * board.h:
 **  BOARD_SLAVE_LOOP_ITERATIONS : the frequency of board sensor data aquisitions, 
 *                         : i.e. 1 time in BOARD_SLAVE_LOOP_ITERATIONS; value 1 would be every loop, but then the q will overflow!
 *  
 * Output : 
 **  Master : will Serial.print the data structs
 **  Slave  : will Serial.print the longest length of the Q each time the lenght increases, as well as some init messages.
 */

#include "app.h"

//#define SLOW_CLOCK

App *app;
  
// SPI interrupt routine must be defined in the Slave, only
ISR (SPI_STC_vect){
  app->SPI_ISR ();
}
  
void setup() {
#ifdef SLOW_CLOCK
    noInterrupts();
    CLKPR = _BV(CLKPCE);  // enable change of the clock prescaler
    CLKPR = _BV(CLKPS0);  // divide frequency by 2
    interrupts();
#endif
    app = new SlaveApp();
    Serial.print("\nsizeof(timeValStruct_t): ");
    Serial.println(sizeof(timeValStruct_t));
    Serial.println("starting up...");
    delay(1000);
}

void loop() {
  app->loop();
}

void serialEvent(){
  app->serialEvent();
}


