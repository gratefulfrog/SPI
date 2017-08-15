#include "app.h"

using namespace std;


char MasterApp::nextChar2Send() const{
  static uint8_t nextIndex = 0;
  const static char letters[] = "abcdefghijklmnopqrstuvwxyz";
  char res = letters[nextIndex++];
  nextIndex = nextIndex == 26 ? 0 : nextIndex;
  return res;
}

MasterApp::MasterApp():App(){
    
  cout << "Master\n";

  // prime the pump
  char buff[bigBuffSize];
  outgoingMsg(buff);
  cout << buff;
    
  unsigned char outgoing  = nextChar2Send();

  // print outgoing character
  cout << "Sent: " << outgoing << endl;
    

  // send outgoing character, ignore resonse, this primes the pump
  transferAndWait (outgoing);  
  //wiringPiSPIDataRW(channel,&outgoing, 1);
  nanosleep(&pauseStruct,NULL);
}

void MasterApp::loop(){  
  unsigned char
    outgoing  = nextChar2Send(),
    sent      = outgoing;
  
  transferAndWait (outgoing);
  //char rec = wiringPiSPIDataRW(channel,&outgoing, 1);
  nanosleep(&pauseStruct,NULL);

  
  cout << "Received: " << outgoing << endl;

  // wait for the user to read the screen!
  nanosleep(&slaveProcessingTime,NULL);
  
  // print heading and send count
  char buff[bigBuffSize];
  outgoingMsg(buff);

  cout << buff << "Sent: " << sent << endl;
}


