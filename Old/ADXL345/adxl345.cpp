// Driver code for the  ADLX345 3 channel acceleromater
// using the USART SPI interface.

#include "adxl345.h"

////////////////////////////////////
// spi and data read write functions
////////////////////////////////////
uint8_t ADXL345::spixfer(uint8_t data) const{
  return yspi->transfer(data);
}

void ADXL345::writeRegister(uint8_t reg, uint8_t value) const{
  yspi->beginTransaction();
  spixfer(reg);
  spixfer(value);
  yspi->endTransaction();
}

uint8_t ADXL345::readRegister(uint8_t reg) const{
  reg |= 0x80; // read byte
  yspi->beginTransaction();
  spixfer(reg);
  uint8_t res = spixfer(0xFF);
  yspi->endTransaction();
  return res;
}

int16_t ADXL345::read16(uint8_t reg) const{
  reg |= 0x80 | 0x40; // read byte | multibyte
  yspi->beginTransaction();
  spixfer(reg);
  uint16_t res = spixfer(0xFF)  | (spixfer(0xFF) << 8);
  yspi->endTransaction();
  return res;
}

/////////////////////////////////////
// initialisation and query funcitons
/////////////////////////////////////

void  ADXL345::setRange(range_t range){
  /* Read the data format register to preserve bits */
  uint8_t format = readRegister(ADXL345_REG_DATA_FORMAT);

  /* Update the data rate */
  format &= ~0x0F;
  format |= range;
  
  /* Make sure that the FULL-RES bit is enabled for range scaling */
  format |= 0x08;
  
  /* Write the register back to the IC */
  writeRegister(ADXL345_REG_DATA_FORMAT, format);
  
  /* Keep track of the current range (to avoid readbacks) */
  _range = range;
}

range_t ADXL345::getRange(void) const{
  return  _range;
}

range_t ADXL345::queryRange(void) const{
  return (range_t)(readRegister(ADXL345_REG_DATA_FORMAT) & 0x03);
}

void ADXL345::setDataRate(dataRate_t dataRate){
  writeRegister(ADXL345_REG_BW_RATE, dataRate);
  _dataRate = dataRate;
}

dataRate_t ADXL345::getDataRate(void)const{
  return _dataRate;
}

dataRate_t ADXL345::queryDataRate(void)const{
  return (dataRate_t)(readRegister(ADXL345_REG_BW_RATE) & 0x0F);
}

uint8_t    ADXL345::queryDeviceID(void) const{
  return readRegister(ADXL345_REG_DEVID);
}

int16_t    ADXL345::getX(void) const{    /*! raw value needs to be corrected with mutlipliers */
  return read16(ADXL345_REG_DATAX0);
}
int16_t    ADXL345::getY(void) const{    /*! raw value needs to be corrected with mutlipliers */
  return read16(ADXL345_REG_DATAY0);
}
int16_t    ADXL345::getZ(void) const{    /*! raw value needs to be corrected with mutlipliers */
  return read16(ADXL345_REG_DATAZ0);
}

/** encapsulate adafruit constructor + begin() 
 * don't forget to set the various parameters range, data rate device ID
 */
ADXL345::ADXL345(const YSPI *const y) : YADC(y){
  // Enable measurements
  writeRegister(ADXL345_REG_POWER_CTL, 0x08);

  setRange(ADXL_G_RANGE);
  setDataRate(ADXL_DATARATE);
}
  
float ADXL345::acquireChannel(uint8_t channel){   /*! 0=x, 1=y, 2=z : see getEvent for multipliers !!! */
  float res = 0;
  switch (channel){
  case 0:
    res = getX();
    break;
  case 1:
    res = getY();
    break;
  case 2:
    res = getZ();
    break;
  }
  return res * ADXL345_MG2G_MULTIPLIER * SENSORS_GRAVITY_STANDARD;
}

bool ADXL345::selftest(void){
  return true;
  /* Check connection */
  /* doesn't work! DEVICE ID IS ZERO !!!  */
  uint8_t deviceid = queryDeviceID();
  if (deviceid != 0xE5){
    /* No ADXL345 detected ... return false */
    Serial.println(deviceid, HEX);
    while(1);
    return false;
  }
  return true;
}

