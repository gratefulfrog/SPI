#ifndef YADC_H
#define YADC_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include "yspi.h"

/** YADC
 * Represents the abstract ADC class
 * can also be used as a basis for any mutli channel device that can provide the 
 * manadatory acquireChannel and selftest methods
 */
class YADC {
 protected:
  const YSPI *const yspi = NULL;   /*!< Points to YMSPI instance to be used in all SPI comms. */
  void selfTestFailed() const ;
  
 public:
    YADC(const YSPI *const y);
    virtual float acquireChannel(uint8_t channel) = 0;
    virtual bool selftest(void) = 0;
};
#endif


