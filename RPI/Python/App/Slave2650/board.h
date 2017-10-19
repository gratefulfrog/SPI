#ifndef BOARD_H
#define BOARD_H

#include <Arduino.h>
#include "guid_24AA025UID.h"
#include "adcMgr.h"
#include "config.h"

/** Class encapsulates board functionality, eg number of ADCS, etc.
 */
class Board{
  protected:
    uint8_t boardUSARTChannels[3]; //<! values for the USART chanels, i.e. channnel 0 is UART_1, etc.
    ADCMgr **adcMgrVec;  /*!< vector of pointers to ADC Mgr instances that the board has */
    
  public:
    const uint8_t nbADCs;  /*!< how many ADCs are connected on this board */
    /** constructor
     *  @param numberADC how many adcs are on the board
     *  @param nbChannelVec an array containing the numer of channels that each ADC has 
     */
     Board(uint8_t numberADC, const uint8_t nbChannelVec[]);
    /** get the board id
     *  @return the board's GUID
     */
    boardID getGUID() const;
    /** get a value for an ADC and Channel pair
     *  @param adcID the adc in question
     *  @param channelID the channel in question
     *  @return the value returned by the adc
     */
    float getValue(uint8_t adcID, uint8_t channelID) const;
    /** for a given adc mgr id, get the number of channels available
     *  @param mgrId an index for the adcMgrVec array
     *  @return the number of adc channels for the selected adc
     */
    uint8_t getMgrNbChannels(uint8_t mgrId) const;

};

#endif

