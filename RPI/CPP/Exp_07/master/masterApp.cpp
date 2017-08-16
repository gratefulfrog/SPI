#include "app.h"
#include <endian.h>

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
  outgoing = nextChar2Send();
  transferAndWait(outgoing); // ignore this return
}

void MasterApp::loop(){
  char buff[bigBuffSize];
  outgoingMsg(buff);
  cout << buff;
  
   // print outgoing character that was just sent
  cout << "Sent: " << outgoing << "\nReceived: ";

  outgoing  = nextChar2Send();

  SPI_readAnything_reprime(*spi, inData, outgoing);

  cout << "\ninData.c0: " << inData.c0;
  cout << "\ninData.c1: " << inData.c1;
  cout << "\ninData.c2: " << inData.c2;
  cout << "\ninData.i0: " << (inData.i0)<< endl;

  delaySeconds(slaveProcessingTime);
}


