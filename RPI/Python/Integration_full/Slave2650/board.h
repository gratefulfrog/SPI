#ifndef BOARD_H
#define BOARD_H

#include <Arduino.h>
#include "guid_24AA025UID.h"
#include "adcMgr.h"
#include "config.h"


class Board{
  protected:
  
    const uint8_t nbADCs;  /*!< how many ADCs are connected on this board */
    uint8_t boardUSARTChannels[3];
    ADCMgr **adcMgrVec;  /*!< vector of pointers to ADC Mgr instances that the board has */
    
  public:
    Board(uint8_t numberADC, uint8_t nbChannelVec[]);
    /** get the board id
     *  @return the board's GUID
     */
    boardID getGUID() const;    

    float getVal(uint8_t adcID, uint8_t channelID) const;
};

#endif

