#include "app.h"

//#define USE_Q

#define NO_Q_DELAY (0)   // MicroSeconds min value that works is 0 !
#define Q_DELAY    (1) // milliSeconds min value that works 1 @ 1M baud !    
#define FULL_SPEED (12100000)
#define M_SPEED     (1000000)
//#define BAUDRATE   M_SPEED
#define BAUDRATE   FULL_SPEED


//////////////////////////////////////////////////////////
//////////// Public Methods        ///////////////////////
//////////////////////////////////////////////////////////

SlaveApp::SlaveApp() {
  SerialUSB.begin(BAUDRATE);
  while(!SerialUSB);
  //I2c.begin();
  
  handShake();

  //setupHBLed();
  
#ifdef USE_Q 
  q = new Q<u8u32f_struct>(nullStruct);
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
  #ifdef DEBUG
    SerialUSB.println("board created");
  #endif
  
  setupADCVectors(board_nbADCS,bordNbChannelVec);
  #ifdef DEBUG
    SerialUSB.println(String("board Guid : ") + String(board->getGUID()));
  #endif
  u8u32f_struct  nextStruct = {255,board->getGUID(),0.0};
  SerialUSB.write((uint8_t*)&nextStruct,sizeof(nextStruct));
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
  while (SerialUSB.available() <=0){
  }
  SerialUSB.read();
}

void SlaveApp::setupHBLed(){
  pinMode(SlaveApp_LED_PIN_1, OUTPUT);
  digitalWrite(SlaveApp_LED_PIN_1, LOW);

  pinMode(SlaveApp_LED_PIN_2, OUTPUT);
  digitalWrite(SlaveApp_LED_PIN_2, LOW);
}

void SlaveApp::stepHB() const{
  return;
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
  if (!q->push(nextStruct)){ // no more room in q !! yes, this value is lost forever...
    res = State::readyToSend;
  }
#else
  SerialUSB.write((uint8_t*)&nextStruct,sizeof(u8u32f_struct));
  delayMicroseconds(NO_Q_DELAY);  
#endif

  ChannelID++; 
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
    SerialUSB.write((uint8_t*)&nextStruct,sizeof(nextStruct));
    delay(Q_DELAY);
    //delayMicroseconds(Q_DELAY);
  }
 
  return res;
}
#endif
