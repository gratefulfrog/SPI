#include "board.h"

Board::Board(){  
  AA025UID g = AA025UID();
  guid = g.getGuidID();
}

boardID Board::getGUID() const{
  return guid;
}
