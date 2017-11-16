/* 
 *  This version uses interrupts only!.
 *  intergates timestamper.
 */

// use serial monitor to observe status

#include "app.h"


/** pointer to instance of SlaveApp that will do the work!
 */
SlaveApp *app;


/** Deinition of the SPI Slave interrupt handler as
 *  app pointers SPI_ISR method
 */
ISR (SPI_STC_vect){
  app->SPI_ISR ();
}
/** setup code instantiates the Slave App
 */
void setup() {
  app = new SlaveApp();
}

/** loop code just calls Slave App loop method
 */
void loop() {
  app->loop();
}
