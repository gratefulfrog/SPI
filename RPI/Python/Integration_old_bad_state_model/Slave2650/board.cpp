#include "board.h"
Board::Board() : guid(BOARD_BOARD_ID){  
}

boardID Board::getGUID() const{
  return guid;
}
