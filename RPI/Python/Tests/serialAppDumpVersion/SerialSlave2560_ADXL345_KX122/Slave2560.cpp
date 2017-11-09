#include "app.h"

#define USE_Q

#define NO_Q_DELAY (0)   // MicroSeconds min value that works is 0 !
#define Q_DELAY    (10) // milliSeconds min value that works 1 @ 1M baud !    
#define BAUDRATE   (1000000)
//////////////////////////////////////////////////////////
//////////// Public Methods        ///////////////////////
//////////////////////////////////////////////////////////

SlaveApp::SlaveApp() {
  Serial.begin(BAUDRATE);
  while(!Serial);
  I2c.begin();
  
  handShake();
  setupHBLed();  
  
#ifdef USE_Q 
  //q = new Q<u8u32f_struct>(nullStruct);
#endif

  const uint8_t board0NBChannelVec[] = {BOARD_BOARD_0_ADC_0_NB_CHANNELS},
                board1NBChannelVec[] = {BOARD_BOARD_1_ADC_0_NB_CHANNELS,
                                        BOARD_BOARD_1_ADC_1_NB_CHANNELS,
                                        BOARD_BOARD_1_ADC_2_NB_CHANNELS,
                                        BOARD_BOARD_1_ADC_3_NB_CHANNELS,
                                        BOARD_BOARD_1_ADC_4_NB_CHANNELS};

  // BOARD_BOARD_0 is board zero with only one ADC
  // BOARD_BOARD_1 is board one with 2 adcs + 1 adxl345
  
  
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
  setupADCVectors(board_nbADCS,bordNbChannelVec);
  u8u32f_struct  nextStruct = {255,board->getGUID(),0.0};
  Serial.write((uint8_t*)&nextStruct,sizeof(nextStruct));
  currentState = State::initialized;
}

void SlaveApp::SlaveApp::loop(){
  /*
  if (previousState != currentState){
    previousState = currentState;
    stepHB();
  }
  */
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
  Serial.read();
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

void SlaveApp::setupADCVectors(uint8_t board_nbADCS, const uint8_t *bordNbChannelVec){
  nbActiveADCS = 0;
  adcIndexVec = new uint8_t[board_nbADCS];

  for (uint8_t i=0;i<board_nbADCS;i++){
    if (bordNbChannelVec[i]>0) { // i represents an active adc
      adcIndexVec[nbActiveADCS++] = i;
    }
  }
}

void SlaveApp::createTimeStamper(){
  if(TimeStamper::theTimeStamper  == NULL){
    TimeStamper::theTimeStamper = new TimeStamper(micros());
  }
}

SlaveApp::State SlaveApp::doWork(){
  static uint8_t ADC_Index = 0;
  static uint8_t ChannelID = 0;
  State res = currentState;

 if (ChannelID == board->getMgrNbChannels(adcIndexVec[ADC_Index])){
    ADC_Index = (ADC_Index +1) %  nbActiveADCS;
    ChannelID = 0;
    return res;
  }
      
  u8u32f_struct  nextStruct = {0,0,0.0};

  encode(nextStruct.u8, adcIndexVec[ADC_Index], ChannelID);
  nextStruct.u32 = TimeStamper::theTimeStamper->getTimeStamp();
  nextStruct.f   = board->getValue(adcIndexVec[ADC_Index],ChannelID);
  
#ifdef USE_Q 
  q[qIndex++] = nextStruct;

  if (qIndex == Q_LEN){ // no more room in q !! yes, this value is lost forever...
    res = State::readyToSend;
  }
#else
  Serial.write((uint8_t*)&nextStruct,sizeof(8u32f_struct));
  delayMicroseconds(NO_Q_DELAY);  
#endif

  ChannelID++; 
  return res;
}


SlaveApp::State SlaveApp::doSingleSend(){
  //digitalWrite(SlaveApp_LED_PIN_2,HIGH);
  //handShake();
  //while (Serial.available() <=0);
  //Serial.read();
  
  //digitalWrite(SlaveApp_LED_PIN_1, HIGH);
  for(uint16_t i=0;i<Q_LEN; i+=100){
    Serial.write((uint8_t*)&q[i],sizeof( u8u32f_struct ) * Q_LEN);
    delay(Q_DELAY);
  }
  qIndex = 0;
  //delay(Q_DELAY);
  //digitalWrite(SlaveApp_LED_PIN_1,LOW);
  //digitalWrite(SlaveApp_LED_PIN_2, LOW);
 
  return State::working;
}

