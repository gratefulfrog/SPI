#include "app.h"

char MasterApp::nextChar2Send() const{
  static byte nextIndex = 0;
  const static char letters[] = {initChar,initChar,initChar,initChar,
                                 bidChar,
                                 acquireChar}; //bcdefghijklmnopqrstuvwxyz";
  char res = letters[nextIndex];
  //nextIndex = (nextIndex == 0) ? 1 : 2;  // set this to 2 to send an acquireChar
  if (++nextIndex >5){
    nextIndex = 5;
  }
  else{
    delay(20);
  }
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
      //Serial.println(slaveTime);
      processReply(slaveTime, true);
      break;
    case bidChar:
      SPI_readAnything_reprime(inBoardID, (byte)nextCommand);
      Serial.println("Command : bidChar");
      //Serial.println(inBoardID);
      processReply(inBoardID,false);
      break;
    default:      
      SPI_readAnything_reprime(inTVS, (byte)nextCommand);
      //Serial.println("Command : otherChar");
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

void MasterApp::serialEvent(){
  if (Serial.available()){
    Serial.read();
    consoleInput = true;
  }
}
 


