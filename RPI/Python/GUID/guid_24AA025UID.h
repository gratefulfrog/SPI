#ifndef AA025UID_H
#define AA025UID_H


#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif


#include "I2C.h"


#define AA025UID_GUID_ADDR   (0xFA)
#define GUID_I2C_ADR         (0x50)
#define GUID_ID_LEN          (6)
#define GUID_SERIAL_NB_START (2)

class AA025UID {
 protected:
  uint32_t guidID;
  const int8_t I2C_addr;
 
 public:
  AA025UID();
  const uint32_t& getGuidID() const;
};

#endif
