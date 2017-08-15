
#include "exp.h"


/*
 * This version sends lowercase characters and receives upper case characters in response.
 * It is interesting to monitor both Master and Slave in a ternminal
 */


int main() {
  App *app = new MasterApp();  
  while(1){
    app->loop();
  }
}
