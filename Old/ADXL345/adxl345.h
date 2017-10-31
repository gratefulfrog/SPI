#ifndef ADL345_H
#define ADL345_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <Adafruit_ADXL345_U.h>
#include "yspi.h"
#include "yADC.h"

#define ADXL_DATARATE    (ADXL345_DATARATE_12_5_HZ)  // value selected by guessing, see Adafruit_ADXL345_U.h for other values
#define ADXL_G_RANGE     (ADXL345_RANGE_2_G)         // other values are _4_G _8_G _16_G
#define ADXL_NB_CHANNELS (3)                        /*! x,y,z */

/**
 * Represents the  ADXL345, acts like an ADC with 3 channels and 16 bit resolution.
 * Encapsulates tha Adafruit driver and integrates YSPI functionality
 */
class ADXL345 : public YADC {
  protected:
    range_t           _range;
    dataRate_t        _dataRate;
    
    // spi and data read write functions
    uint8_t spixfer(uint8_t data) const;
    void    writeRegister(uint8_t reg, uint8_t value) const;
    uint8_t readRegister(uint8_t reg) const;
    int16_t read16(uint8_t reg) const;

    // initialisation and query funcitons
    void       setRange(range_t range);
    range_t    getRange(void) const;
    range_t    queryRange(void) const;
    void       setDataRate(dataRate_t dataRate) ;
    dataRate_t getDataRate(void) const;
    dataRate_t queryDataRate(void) const;
    uint8_t    queryDeviceID(void) const;
    
    int16_t    getX(void) const,    /*! raw value needs to be corrected with mutlipliers */
               getY(void) const,    /*! raw value needs to be corrected with mutlipliers */
               getZ(void) const;    /*! raw value needs to be corrected with mutlipliers */
  public:
    /** encapsulate adafruit constructor + begin() 
     * don't forget to set the various parameters range, data rate device ID
     */
    ADXL345(const YSPI *const y);  
    float acquireChannel(uint8_t channel);   /*! 0=x, 1=y, 2=z : see getEvent for multipliers !!! */
    bool selftest(void);
};
#endif


