/* 
 *  This version uses interrupts only!.
 */


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
