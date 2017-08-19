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
  SPI.begin ();
  while(!Serial);
  Serial.println("Master");

  /* prime the pump for the 1st character */
  outgoing  = nextChar2Send();
  digitalWrite(SS, LOW);   
  delayMicroseconds (pauseBetweenSends);
   
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
      Serial.println("Command : initChar");
      processReply(slaveTime, true);
      break;
    case bidChar:
      SPI_readAnything_reprime(inBoardID, (byte)nextCommand);
      Serial.println("Command : bidChar");
      processReply(inBoardID,false);
      break;
    default:      
      SPI_readAnything_reprime(inTVS, (byte)nextCommand);
      processReply(inTVS);
      break;
  }
}

void MasterApp::loop(){  
  #ifdef DEBUG
    printSendCount();

     // print outgoing character that was just sent
    Serial.print("Sent: ");
    Serial.println(outgoing);
    //Serial.print("Received: ");
  #endif
  static uint32_t loopCount = 0;

  if (!(loopCount++%100000)){
    Serial.print("loop count: ");
    Serial.print(loopCount);
    Serial.print("    bad count: ");
    Serial.println(badCounter);
  }  
  
  char nextOutgoing  = nextChar2Send();
  
  // enable Slave Select
  digitalWrite(SS, LOW);   
  delayMicroseconds (pauseBetweenSends);
  readReplyAndSendNext(outgoing,nextOutgoing);
  outgoing = nextOutgoing;
  
  
  
  // disable Slave Select
  digitalWrite(SS, HIGH);
  delay (slaveProcessingTime);
}

#ifdef DEBUG  
void MasterApp::serialEvent(){
  if (Serial.available()){
    Serial.read();
    consoleInput = true;
  }
}
  #endif
 


