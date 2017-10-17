/* 
 *  This version has #define to either use interrupts or not.
 */


#include "app.h"

//#define USE_INTERRUPTS

/*
 * This version sends lowercase characters and receives upper case characters in response.
 * It is interesting to monitor both Master and Slave in a ternminal
 */

SlaveApp *app;

#ifdef  USE_INTERRUPTS
ISR (SPI_STC_vect){
  app->SPI_ISR ();
}
#endif

void setup() {
  app = new SlaveApp();
}

void loop() {
  app->loop();
}
