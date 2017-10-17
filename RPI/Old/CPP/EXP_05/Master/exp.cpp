
#include "exp.h"


/*
 * This version sends lowercase characters and receives upper case characters in response.
 * It is interesting to monitor both Master and Slave in a ternminal
 */

using namespace std;

int main() {
  int fd = wiringPiSPISetup(0,500000);
  cout << "wiring spi setup result: " << fd << endl;
  
  App *app = new MasterApp();  
  while(1){
    app->loop();
  }
}
