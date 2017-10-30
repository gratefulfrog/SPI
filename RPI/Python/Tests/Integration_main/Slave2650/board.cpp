#include "board.h"

Board::Board(){}

boardID Board::getGUID() const{
  //AA025UID g = AA025UID();
  return AA025UID().getGuidID();
}
