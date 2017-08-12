#include "yspi.h"

/* HW SPI LIB uses the following PIN definitions
 * CLK   = 13 GREEN
 * MIS0  = 12 ORANGE
 * MOSI  = 11 BLUE
 * SS    = 10 YELLOW
 */

HWSPI::HWSPI(uint8_t SSpin, 
             uint32_t clockF, 
             uint8_t bitOrder, 
             uint8_t dataMode) : YSPI(), 
                                 SS(SSpin), 
                                 settings(clockF, bitOrder,dataMode){
  SPI.begin();
  pinMode(SS, OUTPUT);    // configure slave select pin as output (not controlled by SPI transceiver)
  setSS(HIGH);  
}

void HWSPI::setSS(uint8_t highLow) const{
  digitalWrite(SS,highLow);
}

void HWSPI::beginTransaction() const{
  SPI.beginTransaction(settings);
  setSS(LOW);
}

uint8_t HWSPI::transfer (uint8_t data) const{
  return SPI.transfer(data);
}

void HWSPI::endTransaction() const{
  setSS(HIGH);
  SPI.endTransaction();
}


