
#include <stdint.h>
#include <iostream>

#include "fileMgr.h"

int main(){
  const int tSize = 3;
  const timeValStruct_t tvsVec[tSize] = {{1<<4,0,0.01},
					 {1<<4|1,1,1.02},
					 {3<<4|2,2,2.03}};
  uint8_t nbADC = 2,
    channelVec[] = {1,2};
  
  FileMgr * fm = new FileMgr(tSize);
  
  fm->setBID(111);
  fm->setTID();
  
  try  {
    // Data
    for (int i=0;i<tSize;i++){
      fm->addTVS(tvsVec[i]);
    }
    for (int i=0;i<tSize-1;i++){
      fm->addTVS(tvsVec[i]);
    }
    for (int i=0;i<tSize;i++){
      fm->addTVS(tvsVec[i]);
    }
    
  }
  catch (const std::exception& ex){
    std::cout << "Exception was thrown in main: " << ex.what() << std::endl;
  }
  return 0;
}
