
#include "exp.h"


/*
 * This version sends lowercase characters and receives upper case characters in response.
 * It is interesting to monitor both Master and Slave in a ternminal
 * 2017 08 16: This works!
 */

using namespace std;

const int channel = 0,
  speed = 500000;

int main() {
  
  App *app = new MasterApp(channel,speed);  
  while(1){
    app->loop();
  }
}
