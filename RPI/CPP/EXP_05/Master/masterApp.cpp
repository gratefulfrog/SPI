#include "app.h"

using namespace std;


char MasterApp::nextChar2Send() const{
  static byte nextIndex = 0;
  const static char letters[] = "abcdefghijklmnopqrstuvwxyz";
  char res = letters[nextIndex++];
  nextIndex = nextIndex == 26 ? 0 : nextIndex;
  return res;
}

MasterApp::MasterApp():App(){
    
  cout << "Master";

  // prime the pump
  char buff[bigBuffSize];
  outgointMsg(buff);
  cout << buf;
  //Serial.print(buff);
  
  char outgoing  = nextChar2Send();

  // print outgoing character
  cout "Sent: " << outgoing;
  //Serial.print("Sent: ");
  //Serial.println(outgoing);
  
  // enable Slave Select
  //digitalWrite(SS, LOW);
  //delayMicroseconds(20);    

  // send outgoing character, ignore resonse, this primes the pump
  transferAndWait (outgoing);  
  
  // disable Slave Select
  //digitalWrite(SS, HIGH);
  //delayMicroseconds(20);
}

void MasterApp::loop(){  
  char outgoing  = nextChar2Send();
  //Serial.print("Received: ");
  cout << "Received: " << (char)transferAndWait (outgoing) << endl;
  
  // enable Slave Select
  //digitalWrite(SS, LOW);   
  //delayMicroseconds(20); 
 
  // send next outgoing character, receive resonse to previous send
  //Serial.println((char)transferAndWait (outgoing));  

  // disable Slave Select
  //digitalWrite(SS, HIGH);

  nanosleep(&slaveProcessingTime,NULL);
  
  // print heading and send count
  char buff[bigBuffSize];
  outgointMsg(buff);
  //Serial.print(buff);
  cout << buff << "Sent: " << outgoing << endl;
  
  // print outgoing character that was just sent
  //Serial.print("Sent: ");
  //Serial.println(outgoing);
}


