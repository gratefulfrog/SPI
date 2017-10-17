#include "app.h"

void SlaveApp::setupHBLed(){
  pinMode(SlaveApp_LED_PIN_1, OUTPUT);
  digitalWrite(SlaveApp_LED_PIN_1, HIGH);

  pinMode(SlaveApp_LED_PIN_2, OUTPUT);
  digitalWrite(SlaveApp_LED_PIN_2, LOW);
}

void SlaveApp::stepHB(){
  static uint32_t lastHBTime = millis();
  uint32_t now = millis();

  static const uint8_t pinVec[] = {SlaveApp_LED_PIN_1, SlaveApp_LED_PIN_2};
  
  if ((now-lastHBTime > SlaveApp_HB_TIME) || (now < lastHBTime)) {
    for (uint8_t i=0;i<2;i++){
     digitalWrite(pinVec[i],!digitalRead(pinVec[i]));
    }
   lastHBTime = now;
  }
}

SlaveApp::SlaveApp() {
  Serial.begin(115200);
  setupHBLed();
  Serial.println("Ready!");
  q = new Q<u8u32f_struct>(nullStruct);

  uint8_t board0NBChannelVec[] = {BOARD_BOARD_0_ADC_0_NB_CHANNELS};

  board = new Board(1,board0NBChannelVec);

  for (int i=0 ; i< BOARD_BOARD_0_ADC_0_NB_CHANNELS; i++){
    Serial.println(String("Board[0]. ADCMgr[0], Channel[") + 
                   String(i) + 
                   String("] : Value : ") +
                   String(board->getValue(0,i)));
  }

  initResponseStruct.u32 = board->getGUID();
  Serial.println(String("BID : ") + String(initResponseStruct.u32));
  
   
  /* Set MISO output, all others input */
  pinMode(MISO,OUTPUT);
  
  /* Enable SPI in SLAVE mode */
  SPCR = (1<<SPE);

  // turn on interrupts
  SPCR |= _BV(SPIE);
  currentState = State::started;
}

#define SAY_HEARTBEAT_ONLY (true)
#define HB_SHOW_COUNT      (300)

void SlaveApp::sayState(){
  static const boolean hbOnly = SAY_HEARTBEAT_ONLY;
  static uint32_t counter = 0;

  if (hbOnly){
    if (!(counter++ % HB_SHOW_COUNT)){
      Serial.println(String("hearbeat : ") + String(counter));
    }
    return;
  }
  
  String msg = String(int(currentState)) + String(" : "); "started";
  switch (currentState){
    case State::unstarted:
      msg += String("unstarted");
      break;
    case State::started:
      msg += String("started");
      break;
    case State::initialized:
      msg += String("initialized");
      break;
    case State::working:
      msg += String("working");
      break;
    case State::sendingStructs:
      msg += String("sending Structs");
      break;
    case State::readyToSend:
      msg += String("Ready To Send");
      break;
  }
  Serial.println(String("Current State : ") + msg);
}
/*  State achine
- started, expecting SPI init
- initialized, then immediately working,
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
  stepHB();
  // if the flag is true, then an interrupt has requested a state change.
  // This could be either we got an init8 poll and were in State::started, and next is State:initialized
  // or we got a payload8 and either we are readyToSend or already sendingStructs, then
  // if there is stuff to send, we go to sendingStructs, 
  // if not we go to working.
  // the flag is only true if there nextState != currentState
  // Truth table:
  /*
      Interrupt   currentState     result of incOutgoing   nextState      flag (nextState != currentState)
      init8       started          N/A                     initialized    TRUE
      payload8    readyToSend      TRUE                    sendingStructs TRUE
      payload8    readyToSend      FALSE                   working        TRUE  // this should never happen!
      payload8    sendingStructs   TRUE                    sendingStructs FALSE
      payload8    sendingStructs   FALSE                   working        TRUE  
  */
  // so the flag will be set to true, but there will always be a wait of at least 0.1 secs before it is set again to true, 
  // which is why we don't need to worry about the flag changing between the evaluation of the 'if'
  // and the flag being set to false.
  // suppose that were to happen, i.e. flag is true, the master always waits at least 0.1 seconds before sending a new SPI poll
  // so suppose flag is set to true just after the if, the entire rest of the loop must take longer than 0.1 secs if it is to 
  // be set to true again, before the if is re-evaluated. This is not possile, I think
  if (flag){
    currentState=nextState;
    flag = false;
  }
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
    // case State::started:      // SPI init poll will set flag causing change of state
    case State::initialized:    
      createTimeStamper();
      currentState = State::working;
      break;   
     case State::working:
      noInterrupts();   // no interruptions while working
      currentState =  doWork();  // end of work will cause change of state to readyToSend
      break;
    case State::readyToSend:    // we are done working, spi payload poll will set flag causing change of state to sendingStructs
      interrupts();
      break;
    //case State::sendingStructs:  // SPI Payload poll will set flag causing change of state to working
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
  // got the payload poll
  else if ((type == payload8) && 
           ((currentState == State::readyToSend) || (currentState == State::sendingStructs))){  // we got a request for payload
      nextState = ((incOutgoing()) ? State::sendingStructs : State::working);
      res = &payload;
  }
  else{  // what could this be???
    while(1);
  }
  return res;
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
    if(currentState != nextState){
      flag = true;
    }
    i=0;
  }
  return res;
}

SlaveApp::State SlaveApp::doWork(){
  static uint8_t ADC_ID = 0;
  static uint8_t ChannelID = 0;

  u8u32f_struct  nextStruct = {0,0,0.0};

  encode(nextStruct.u8, ADC_ID, ChannelID);
  nextStruct.u32 = TimeStamper::theTimeStamper->getTimeStamp();
  nextStruct.f   = board->getValue(ADC_ID,ChannelID);
  
  State res = currentState;

  if (q->push(nextStruct)){  // we could push it onto the q   
    ChannelID = (ChannelID +1 ) % board->getMgrNbChannels(ADC_ID);
    if(!ChannelID){  // we must inc the ADC_id
      ADC_ID = (ADC_ID +1) % board->nbADCs;
    }
  }
  else { // no more room in q !!
    res = State::readyToSend;
  }
  return res;
}

