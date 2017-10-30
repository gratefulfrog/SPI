#include "app.h"

/*
 * This version sends lowercase characters and receives upper case characters in response.
 * It is interesting to monitor both Master and Slave in a ternminal
 */

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
