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
}

void MasterApp::loop(){  
  unisgned char outgoing  = nextChar2Send();
  char buff[bigBuffSize];
  outgointMsg(buff);
  cout << buff;
  
   // print outgoing character that was just sent
  cout << "Sent: " << outgoing << "\nReceived: ";

  char ignored = transferAndWait(outgoing);
  spi->readAnything(inData);

  cout << "\ninData.c0: " << inData.c0;
  cout << "\ninData.c1: " << inData.c1;
  cout << "\ninData.c2: " << inData.c2 << endl;
  
  delay (slaveProcessingTime);
}


