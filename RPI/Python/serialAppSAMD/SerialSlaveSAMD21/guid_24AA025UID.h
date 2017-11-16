#ifndef AA025UID_H
#define AA025UID_H

#include <Arduino.h>
#include <Wire.h>

#define AA025UID_GUID_ADDR   (0xFA)
#define GUID_I2C_ADR         (0x50)
#define GUID_ID_LEN          (6)
#define GUID_SERIAL_NB_START (2)


/** class provides driver fo the AA025UID chip
 */
class AA025UID {
 protected:
  uint32_t guidID;  //<! the gunique GUID value on 4 bytes, not including manufacturer etc.
  const int8_t I2C_addr;  //<! I2C address of the chip
 
 public:
 /** instance constructor
  */
  AA025UID();
  /** provides the 4 byte GUID
   *  @return const reference to 32bit GUID
   */
  const uint32_t& getGuidID() const;
};

#endif
