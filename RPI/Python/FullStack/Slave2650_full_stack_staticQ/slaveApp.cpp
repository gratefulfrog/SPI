#include "app.h"

SlaveApp::SlaveApp() {
  Serial.begin(115200);
  Serial.println("Ready!");
  q = new Q<u8u32f_struct>(nullStruct);
  
  /* Set MISO output, all others input */
  pinMode(MISO,OUTPUT);
  
  /* Enable SPI in SLAVE mode */
  SPCR = (1<<SPE);

  // turn on interrupts
  SPCR |= _BV(SPIE);
}

void SlaveApp::sayState(){
  String msg = "started";
  switch (currentState){
    case State::initialized:
      msg = "initialized";
      break;
    case State::bidSent:
      msg = "Bid Sent";
      break;
    case State::readyToWork:
      msg = "Ready to Work";
      //saidWorking = false;
      break;
    case State::working:
      msg = "working";
      //saidWorking=true;
      break;
    case State::sendingStructs:
      //saidWorking = false;
      msg = "sending Structs";
      break;
    case State::readyToSend:
      msg = "Ready To Send";
      break;
  }
  //Serial.println(String("Current State : ") + msg);
}

void SlaveApp::doTests(){
  static unsigned int count = 1;
  if ((previousState == State::sendingStructs) && 
      (currentState   ==  State::working)){
        Serial.println(String("*** STATE ERROR : ") + String(count++)); // error but how???
        currentState = State::readyToWork;
      }
}

void SlaveApp::SlaveApp::loop(){
  boolean changed = false;
  if (previousState != currentState){
    doTests();
    previousState = currentState;
    sayState();
    changed=true;
  }
  else if (currentState == State::bidSent){
    currentState = State::readyToWork;
  }
  else if (currentState == State::readyToWork){
    currentState = State::working;
  }
  else if (currentState == State::working){
    noInterrupts();
    currentState = doWork();
  }
  else if (currentState==State::readyToSend){
    interrupts();
  }
}


void SlaveApp::SPI_ISR(){
  if(!isSlaveMsg(SPDR)){
    SPDR = response(SPDR);
  }
}

boolean SlaveApp::incOutgoing(){
  // return true if more data is vailable, false otherwise
  // only call this when there is no enqueueing activitiy
  payload =  q->pop();
  return (payload != nullStruct);
}

boolean SlaveApp::isSlaveMsg(byte msg) const{
  return !(msg & (0b1111<<4));
}

u8u32f_struct* SlaveApp::getOutgoing(uint8_t type) {
  if (type == init8){
    nextState = State::initialized;
    return &initResponseStruct;
  }
  else if (type == bid8){
    nextState = State::bidSent;
    return &bidResponseStruct;
  }
  else if (type == wakeUp8){
    // stop working and get ready to send all the data back
    nextState = State::sendingStructs;
    return &nullStruct;
  }
  /*
  else if (type == startWork8){
    // start working, after a payload send sequence
    nextState = ((currentState == State::sendingStructs) ? State::readyToWork : State::working);
    return &nullStruct;
  }
  */
  else if (type == payload8) {  // we got a request for payload
    if (!incOutgoing()){
      nextState = State::readyToWork;
    }
    else{
      nextState = State::sendingStructs;
    }
    return &payload;
  }
  else{  // what could this be???
    while(1);
  }
}

byte SlaveApp::response(uint8_t incoming){
  byte res = 0;
  static int i =0;
  
  // if we get a new Type, i.e. 1000 0000, then we start a new send of outgoing
  if (incoming & 0b10000000){
    outgoing = getOutgoing(incoming);
    bytePtr = (uint8_t*)  outgoing;
    res = (*(bytePtr+i))>>4 & 0b1111; // send 1st 1/2 byte of outgoing
  }
  else if (incoming == 0b10000){ // send second 1/2 byte of outgoing
    res = (*(bytePtr+i++) & 0b1111);
  }
  else if (incoming == 0b100000){ // send 1st 1/2 byte of outgoing
    res = (*(bytePtr+i))>>4 & 0b1111; // send 1st 1/2 byte of outgoing
  }
  else { // it's filler
    currentState = nextState;
    i=0;
  }
  return res;
}

SlaveApp::State SlaveApp::doWork(){
  static u8u32f_struct  nextStruct = {0,0,0.0};
  static uint32_t counter = 0;  
  //static boolean saidQFull = false;

  State res = currentState;

  if (q->push(nextStruct)){  // we could push it onto the q
    /*
    counter++;
    Serial.println(String("Enqueue : ") + String(counter) +
                   String("\t") + String(nextStruct.u8) + 
                   String(" ") + String(nextStruct.u32) + 
                   String(" ") + String(nextStruct.f)  
                   );
    */
    nextStruct.u8++;
    nextStruct.u32++;
    nextStruct.f += 0.01;
    if (nextStruct.f >= maxFloat){
        nextStruct.f=0.0;
    }
  }
  else { // no more room in q !!
    //Serial.println("Q Full, work stopped!");
    res = State::readyToSend;
  }
  return res;
}

/*  obsolete code
void SlaveApp::doWork(){
  static uint32_t counter = 0;   
  Serial.print(String("Working :") + String(counter++));
  uint32_t i = 0;
  for (; i < NB_WORK_LOOPS;){
  i++;
  }
  Serial.println(String("...")+String(i));
}
*/

