#ifndef YADLX345_H
#define YADLX345_H

#include <Arduino.h>
#include <SparkFun_ADXL345.h>

#include "yspi.h"
#include "yADC.h"

// SPARFUN define !!
#define ADXL345_TO_READ (6)      // Number of Bytes Read - Two Bytes Per Axis


#define YADXL345_NB_CHANNELS (3)         /*! x,y,z */
#define YADXL345_T_DELAY     (15)        /*! time afer CS goes low before transaction can begin */
#define YADXL345_T_QUIET     (15)        /*! time after transfer before SC goes HIGH */
/**
 * Represents the  ADXL345, acts like an ADC with 3 channels and 16 bit resolution.
 * Encapsulates tha Adafruit driver and integrates YSPI functionality
 */
class YADXL345 : public YADC {
 protected:
  byte _buff[ADXL345_TO_READ];		      //	6 Byte Buffer

  void powerOn() const;
  void writeToSPI(byte __reg_address, byte __val) const;
  void readFromSPI(byte __reg_address, int num, byte _buff[]) const;
  void setRegisterBit(byte regAdress, int bitPos, bool state) const;
  void setSpiBit(bool spiBit) const;
  void setRangeSetting(int val) const;
  void readAccel(int* x, int* y, int* z);
  
 public:
  YADXL345(const YSPI *const y);  
  float acquireChannel(uint8_t channel);   /*! 0=x, 1=y, 2=z */
  bool selftest(void);
};
#endif


