#ifndef BOARD_H
#define BOARD_H

#include <Arduino.h>
#include "adcMgr.h"
#include "q.h"
#include "informer.h"

// How many data structs will be sent back to the Master at every ACQUIRE query
#define OUTPUT_BURST_LENGTH   (Q_LENGTH)
// set the following value to (1) for maximum slave speed!
// with no Serial Outoput :
//   * 10 loop iters makes for a loop time for the 2 sensors of 1827 microseconds
//   * 5  looper iters = 1032 microseconds!
//   * 4  is too fast and does not work!
#define SLAVE_LOOP_ITERATIONS (5)  // 5 // 10 // 20 // 40  // 80 //160  

typedef unsigned long boardID; // GUID

class Board{
  protected:
    static const uint8_t boardNbOfADCS[];  // board 0 has 2 adcs
    static const uint8_t boardInformerPin[]; 
    
    const Informer *informer;
    unsigned long counter = 0;  // for simulation
    const int nbDataGets;       
    
    const uint8_t nbADCs;
    const boardID guid;

    char incomingChar;
    boolean incomingFlag = false;

    TimeStamper *ts =  NULL;
    
    ADCMgr **adcMgrVec;
    //Sensor **sensorVec;
    Q<timeValStruct_t> *q;

    //void updateSensorData();
    void updateADCData();
    void getData();

  public:
    static const timeValStruct_t nullReturn; 
    
    Board(boardID iid);
    boardID getGUID() const;
    void loop();  
    timeValStruct_t *pop();
    void showQSize() const;
    void  setT0(timeStamp_t time0);
    void clearQ();
    unsigned int getQSize() const;
};

#endif
