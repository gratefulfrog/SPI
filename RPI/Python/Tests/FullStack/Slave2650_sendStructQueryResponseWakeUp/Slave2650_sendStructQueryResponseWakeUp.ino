/* 
 *  This version uses interrupts only!.
 */

// use serial monitor to observe satatus

#include "app.h"

SlaveApp *app;

ISR (SPI_STC_vect){
  app->SPI_ISR ();
}

void setup() {
  app = new SlaveApp();
}

void loop() {
  app->loop();
}
