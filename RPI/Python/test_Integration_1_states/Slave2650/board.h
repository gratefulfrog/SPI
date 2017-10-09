#ifndef BOARD_H
#define BOARD_H

#include <Arduino.h>

#include "config.h"


class Board{
  protected:
    const boardID guid;    /*!< board id, as guid, needs to be redone when the real GUID functionality is available */
    
  public:
    Board();  
    /** get the board id
     *  @return the board's GUID
     */
    boardID getGUID() const;    
};

#endif

