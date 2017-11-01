#ifndef YADC_H
#define YADC_H

#include <Arduino.h>

#include "yspi.h"


/**
 * Represents the abstract ADC class
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


