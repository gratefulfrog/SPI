#include "app.h"

char MasterApp::nextChar2Send() const{
  static int8_t nextIndex = 0;
  const int8_t nbChars = 3;
  const static char letters[] = {initChar,
                                 bidChar,
                                 acquireChar}; 
  char res = letters[nextIndex];
  nextIndex = (nextIndex == nbChars -1) ? nbChars -1 : nextIndex+1;  
  return res;
}

MasterApp::MasterApp():App(){  
  Serial.println("Master");

  /* prime the pump for the 1st character */
  outgoing  = nextChar2Send();   
  transferAndWait(outgoing);  // ignore this reply!  
}

void MasterApp::readReplyAndSendNext(char command, char nextCommand){
  static uint32_t inBoardID;
  static uint32_t slaveTime;
  static timeValStruct_t inTVS;
  
  switch (command){
    case initChar:
      // set time to zero and clear the board Q
      SPI_readAnything_reprime(slaveTime, (byte)nextCommand);
      cout << "Command : initChar" << endl;
      processReply(slaveTime, true);
      break;
    case bidChar:
      SPI_readAnything_reprime(inBoardID, (byte)nextCommand);
      cout << "Command : bidChar" << endl;
      processReply(inBoardID,false);
      break;
    default:      
      SPI_readAnything_reprime(inTVS, (byte)nextCommand);
      processReply(inTVS);
      break;
  }
}

void MasterApp::loop(){  
  char nextOutgoing  = nextChar2Send();
  
  readReplyAndSendNext(outgoing,nextOutgoing);
  outgoing = nextOutgoing;

  nanosleep(&pauseStruct,NULL);
}


