
#include "exp.h"


/*
 * This version sends lowercase characters and receives upper case characters in response.
 * It is interesting to monitor both Master and Slave in a ternminal
 * 2017 08 16 @ 17:25 This works on RPI with tight data structs! 
 * NOTE:
 **  SPI Speed has been slowed to 100000
 */

using namespace std;

const int channel = 0,
  speed = 100000;

int main() {
  cout << "sizeof (struct DataStruct): " << sizeof (struct DataStruct) << endl;
  
  App *app = new MasterApp(channel,speed);  
  while(1){
    app->loop();
  }
}
