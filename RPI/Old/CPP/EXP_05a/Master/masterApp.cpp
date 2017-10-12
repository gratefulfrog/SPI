#include "app.h"

using namespace std;


char MasterApp::nextChar2Send() const{
  static uint8_t nextIndex = 0;
  const static char letters[] = "abcdefghijklmnopqrstuvwxyz";
  char res = letters[nextIndex++];
  nextIndex = nextIndex == 26 ? 0 : nextIndex;
  return res;
}

MasterApp::MasterApp(int ch, int sp):App(ch,sp){
    
  cout << "Master\n";

  // prime the pump
  char buff[bigBuffSize];
  outgoingMsg(buff);
  cout << buff;
    
  unsigned char outgoing  = nextChar2Send();

  // print outgoing character
  cout << "Sent: " << outgoing << endl;
    
  // send outgoing character, ignore response, this primes the pump
  transferAndWait (outgoing);  
}

void MasterApp::loop(){  
  unsigned char
    outgoing  = nextChar2Send(),
    sent      = outgoing;
  
  cout << "Received: " << transferAndWait (outgoing) << endl;

  // wait for the user to read the screen!
  delay(slaveProcessingTime);
  
  // print heading and send count
  char buff[bigBuffSize];
  outgoingMsg(buff);

  cout << buff << "Sent: " << sent << endl;
}


