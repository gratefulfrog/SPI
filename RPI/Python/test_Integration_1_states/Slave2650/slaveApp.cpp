#include "app.h"

SlaveApp::SlaveApp() {
  Serial.begin(115200);
  Serial.println("Ready!");
  q = new Q<u8u32f_struct>(nullStruct);

  board = new Board();
  bidResponseStruct.u32 = board->getGUID();
  
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
/*  State achine
- started, expecting SPI init
- initialized, expecting SPI bid,
- bid sent, then immediately working,
While true:
- working until Q full,
- ready to send, expecting SPI payload
- sending, until Q empty, discovered in SPI payload op
*/

void SlaveApp::fixCurrentState(){
  static unsigned int count = 1;
  State correctCurrentState;

  switch(previousState){
    case State::unstarted:
      correctCurrentState = State::started;
      break;
    case State::started:
      correctCurrentState = State::initialized;
      break;
    case State::initialized:
      correctCurrentState = State::bidSent;
      break;
    case State::bidSent:
      correctCurrentState = State::working;
      break;
     case State::working:
      correctCurrentState = State::readyToSend;
      break;
    case State::readyToSend:
      correctCurrentState = State::sendingStructs;
      break;
    case State::sendingStructs:
      correctCurrentState = State::working;
      break; 
  }
  if (currentState != correctCurrentState){
    Serial.println(String("*** STATE ERROR : ") + String(count++));
    Serial.println(String("currentState :") + String(int(currentState)) + String("    correctCurrentState :") + String(int(correctCurrentState))); 
    //currentState = correctCurrentState;
  }
}

void SlaveApp::createTimeStamper(){
  if(TimeStamper::theTimeStamper  == NULL){
    TimeStamper::theTimeStamper = new TimeStamper(micros());
    initResponseStruct.u32 = TimeStamper::theTimeStamper->t0;
    Serial.println("Time Stamper created");
  }
}

void SlaveApp::SlaveApp::loop(){
  if (previousState != currentState){
    fixCurrentState();
    previousState = currentState;
    sayState();
  }
  // here there has not been a change of state
  else {
    switch(currentState){
       case State::unstarted:
      currentState = State::started;
      break;
    // case State::started:      // SPI init poll changes state
    case State::initialized:  // SPI BID poll changes state to bid Sent
      createTimeStamper();
      break;   
    case State::bidSent:
      currentState = State::working;
      break;
     case State::working:
      noInterrupts();
      currentState =  doWork();
      break;
    case State::readyToSend:    // SPI Payload poll changes state
      interrupts();
      break;
    //case State::sendingStructs:  // SPI Payload poll changes state
    }
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
  u8u32f_struct* res = &nullStruct;
  nextState = currentState;
  
  // got the INIT SPI poll
  if ((type == init8) && (currentState==State::started)){
    nextState = State::initialized;
    res = &initResponseStruct;
  }
  // got the BID poll
  else if ((type == bid8)  && (currentState == State::initialized)){
    nextState = State::bidSent;
    res = &bidResponseStruct;
  }
  /*else if (type == wakeUp8){
    // stop working and get ready to send all the data back
    nextState = State::sendingStructs;
    return &nullStruct;
  }
  */
  // got the payload poll
  else if ((type == payload8) && 
           ((currentState == State::readyToSend) || (currentState == State::sendingStructs))){  // we got a request for payload
      nextState = ((incOutgoing()) ? State::sendingStructs : State::working);
      res = &payload;
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
  static u8u32f_struct  nextStruct = {0,
                                      TimeStamper::theTimeStamper->getTimeStamp(),
                                      0.0};
  //static uint32_t counter = 0;  
  //static boolean saidQFull = false;
  //static boolean stopped = true;
  //static uint32_t startTime = micros();
  
  State res = currentState;

  /*
  if (stopped){
    startTime = micros();
    stopped = false;
  }
  */

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
    //nextStruct.u32++;
    nextStruct.u32 = TimeStamper::theTimeStamper->getTimeStamp();
    nextStruct.f += 0.01;
    if (nextStruct.f >= maxFloat){
        nextStruct.f=0.0;
    }
  }
  else { // no more room in q !!
    //double rate = (micros()-startTime)/(double)q->qLenght();
    //stopped = true;
    //Serial.println(String("Sampling rate samples/us : ") +String(rate));
    //Serial.println("Q Full, work stopped!");
    res = State::readyToSend;
  }
  return res;
}

