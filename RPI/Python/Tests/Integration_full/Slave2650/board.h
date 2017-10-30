#ifndef BOARD_H
#define BOARD_H

#include <Arduino.h>
#include "guid_24AA025UID.h"
#include "adcMgr.h"
#include "config.h"


class Board{
  protected:
    uint8_t boardUSARTChannels[3];
    ADCMgr **adcMgrVec;  /*!< vector of pointers to ADC Mgr instances that the board has */
    
  public:
    const uint8_t nbADCs;  /*!< how many ADCs are connected on this board */
    Board(uint8_t numberADC, uint8_t nbChannelVec[]);
    /** get the board id
     *  @return the board's GUID
     */
    boardID getGUID() const;
    float getValue(uint8_t adcID, uint8_t channelID) const;
    uint8_t getMgrNbChannels(uint8_t mgrId) const;

};

#endif

