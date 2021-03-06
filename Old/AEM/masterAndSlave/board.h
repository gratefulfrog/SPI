#ifndef BOARD_H
#define BOARD_H

#include <Arduino.h>

#include "config.h"
#include "adcMgr.h"
#include "q.h"
#include "timeStamper.h"


class Board{
  protected:
    static const uint8_t boardNbOfADCS[];  /*!< for all boards; board 0 has 2 adcs */
    uint8_t boardUSARTChannels[];  /*!<  per instance, needs to be initialize in constructor to the actual UARDT channels to use */
    
    unsigned long counter = 0;  /*!< for slave board iteration counts in combination with BOARD_SLAVE_LOOP_ITERATIONS */
    const int nbDataGets;       /*!< this is the number of elts that will be poped off the queue at each poll, see BOARD_OUTPUT_BURST_LENGTH */
    
    const uint8_t nbADCs;  /*!< how many ADCs are connected on this board */
    const boardID guid;    /*!< board id, as guid, needs to be redone when the real GUID functionality is available */
    
    ADCMgr **adcMgrVec;  /*!< vector of pointers to ADC Mgr instances that the board has */
    
    Q<timeValStruct_t> *q;  /*!< the queue to hold the ADC values read until they can be sent to the master */
    
  public:
    /** this is a null value struct used because the Master must always get a reply to his poll, even if there is no data available.
     *  
     *  There will be no data available if the board is looping without polling the adcs because it needs to wait to allow the master
     *  toe collect the data from the queue. */
    static const timeValStruct_t nullReturn; 
    /** Coonstructor initializes the queue,  the  vector of  adc Mgrs, and the  boardUSARTChannels vector  */
    Board();  
    /** get the board id
     *  @return the board's GUID
     */
    boardID getGUID() const;
    /** runs the adc mgr's loop method for each adc mgr, then does nothing for BOARD_SLAVE_LOOP_ITERATIONS - nbADCs */
    void loop();  
    /** pops the queue with noInterrupts()/interrupts() protection of the q methods
     *  
     *  pops the q, and copies the result to a static timeValStruct to be used to send to master,
     *  
     *  Deletes the popped elt if there is one
     *  
     *  @return poitner to a static timeValStruct_t, or NULL if none is available
     *  
     */
    timeValStruct_t *pop();
    void showQSize() const; /*!< Serial prints q size */
    void clearQ();  /*!< pops and deletes all elts in the queue */
    /** gets the q size
     *  @return the nb of elts in the q. */
    unsigned int getQSize() const; 
};

#endif

