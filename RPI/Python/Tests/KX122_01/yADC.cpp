#include "yADC.h"

/**
 * Represents the abstract ADC class
 */
YADC::YADC(const YSPI *const y) : yspi(y){}

void YADC::selfTestFailed() const {
  while (true){
    //yspi->beginTransaction();
    delay(500);
    //yspi->endTransaction();
    delay(500);
  }
}



