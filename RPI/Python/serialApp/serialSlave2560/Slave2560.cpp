#include "app.h"

#define USE_Q

#define NO_Q_DELAY (0)   // MicroSeconds min value that works is 0 !
#define Q_DELAY    (1) // milliSeconds min value that works 1 @ 1M baud !    
#define BAUDRATE   (1000000)
//////////////////////////////////////////////////////////
//////////// Public Methods        ///////////////////////
//////////////////////////////////////////////////////////

SlaveApp::SlaveApp() {
  Serial.begin(BAUDRATE);
  while(!Serial);
  handShake();

  setupHBLed();
  
#ifdef USE_Q 
  q = new Q<u8u32f_struct>(nullStruct);
#endif

  const uint8_t board0NBChannelVec[] = {BOARD_BOARD_0_ADC_0_NB_CHANNELS},
                board1NBChannelVec[] = {BOARD_BOARD_1_ADC_0_NB_CHANNELS,
                                        BOARD_BOARD_1_ADC_1_NB_CHANNELS};

  // BOARD_BOARD_0 is board zero with only one ADC
  // BOARD_BOARD_1 is board one with 2 adcs
  
  
  #if BOARD_USE_BOARD_0
  
  const uint8_t board_ID         = BOARD_BOARD_0_ID,
                board_nbADCS     = BOARD_BOARD_0_NB_ADCS,
               *bordNbChannelVec = board0NBChannelVec;
  
  #else
  
  const uint8_t board_ID         = BOARD_BOARD_1_ID,
                board_nbADCS     = BOARD_BOARD_1_NB_ADCS,
               *bordNbChannelVec = board1NBChannelVec;
  
  #endif
  
  board = new Board(board_nbADCS,bordNbChannelVec);
  
  u8u32f_struct  nextStruct = {255,board->getGUID(),0.0};
 
  Serial.write((uint8_t*)&nextStruct,sizeof(nextStruct));
  
  currentState = State::initialized;
}

void SlaveApp::SlaveApp::loop(){
  if (previousState != currentState){
    previousState = currentState;
    stepHB();
  }
  // here change of state is done
  switch(currentState){
  case State::unstarted:
    currentState = State::started;
    break;
  case State::started:
    currentState = State::initialized;
    break;
  case State::initialized:    
    createTimeStamper();
    currentState = State::working;
    break;   
  case State::working:
    currentState =  doWork();  // end of work will cause change of state to readyToSend
    break;
#ifdef USE_Q 
  case State::readyToSend:    // we are done working, spi payload poll will set flag causing change of state to sendingStructs
    currentState = doSingleSend();
    break;
#endif
  }
}

//////////////////////////////////////////////////////////
//////////// end Public Methods    ///////////////////////
//////////////////////////////////////////////////////////
void SlaveApp::handShake(){
  while (Serial.available() <=0){
  }
}

void SlaveApp::setupHBLed(){
  pinMode(SlaveApp_LED_PIN_1, OUTPUT);
  digitalWrite(SlaveApp_LED_PIN_1, LOW);

  pinMode(SlaveApp_LED_PIN_2, OUTPUT);
  digitalWrite(SlaveApp_LED_PIN_2, LOW);
}

void SlaveApp::stepHB() const{
  static const uint8_t pinVec[] = {SlaveApp_LED_PIN_1, SlaveApp_LED_PIN_2};
  uint8_t sCount = (int)currentState > 2 ?(int)currentState -2 : 0;
  for (uint8_t i=0;i<2;i++){
     digitalWrite(pinVec[i], (i+1) & sCount);
  }
}

void SlaveApp::createTimeStamper(){
  if(TimeStamper::theTimeStamper  == NULL){
    TimeStamper::theTimeStamper = new TimeStamper(micros());
  }
}

SlaveApp::State SlaveApp::doWork(){
  static uint8_t ADC_ID = 0;
  static uint8_t ChannelID = 0;

  u8u32f_struct  nextStruct = {0,0,0.0};

  encode(nextStruct.u8, ADC_ID, ChannelID);
  nextStruct.u32 = TimeStamper::theTimeStamper->getTimeStamp();
  nextStruct.f   = board->getValue(ADC_ID,ChannelID);
 
  State res = currentState;
#ifdef USE_Q 
  if (q->push(nextStruct)){  // we could push it onto the q   
    ChannelID = (ChannelID +1 ) % board->getMgrNbChannels(ADC_ID);
    if(!ChannelID){  // we must inc the ADC_id
      ADC_ID = (ADC_ID +1) %  board->nbADCs;
    }
  }
  else { // no more room in q !!
    res = State::readyToSend;
  }
#else
  //comms->send(nextStruct); 
  Serial.write((uint8_t*)&nextStruct,sizeof(8u32f_struct));
  delayMicroseconds(NO_Q_DELAY);  
  ChannelID = (ChannelID +1 ) % board->getMgrNbChannels(ADC_ID);
  if(!ChannelID){  // we must inc the ADC_id
    ADC_ID = (ADC_ID +1) % board->nbADCs;
  }
#endif
  return res;
}
#ifdef USE_Q 
SlaveApp::State SlaveApp::doSingleSend(){
  State res = currentState;
  u8u32f_struct  nextStruct = q->pop();

  if(nextStruct == nullStruct){
    res = State::working;
  }
  else{
    Serial.write((uint8_t*)&nextStruct,sizeof(nextStruct));
    delay(Q_DELAY);
    //delayMicroseconds(Q_DELAY);
  }
 
  return res;
}
#endif
