/* 
 *  This version uses pure serial only (no OSC)!.
 *  .
 */

// DO NOT use serial monitor to observe status

#include "app.h"

/** pointer to instance of SlaveApp that will do the work!
 */
SlaveApp *app;


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
