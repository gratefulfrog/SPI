#include "kx122.h"

const byte YKX122::regVec[][2] = {{KX122_XOUT_L,KX122_XOUT_H},
				                          {KX122_YOUT_L,KX122_YOUT_H},
				                          {KX122_ZOUT_L,KX122_ZOUT_H}};

void YKX122::configKX122() const{
  byte regVal[][2] = {{KX122_CNTL1,  KX122_HI_RES | KX122_8G},        // set to high res and +/-8g mode
                      {KX122_ODCNTL, KX122_DR_1600},                 // set output data rate 1600Hz
                      {KX122_CNTL1,  KX122_OPERATE}};                // set operational mode
  
  for (int j=0;j<2;j++){
    for(int i=0;i<3;i++){
      I2c.write(addr,regVal[i][0],regVal[i][1]);
    }
  }
}

YKX122::YKX122(uint8_t address, const YSPI *const y)  : YADC(y),addr(address){
  configKX122();
}

float YKX122::acquireChannel(uint8_t channel) {
  I2c.write(addr, regVec[channel][0]);
  uint8_t bytes[2];
  for (uint8_t i=0;i<2;i++){
    I2c.read(addr, regVec[channel][i],1,&bytes[i]);
  }
  return (float)((bytes[1]<<8)|bytes[0]);
}
  
bool YKX122::selftest(void) {
    I2c.write(addr,KX122_WHOAMI);
    I2c.read(addr,1);
    while (!I2c.available());
    return (I2c.receive() == KX122_I_AM_WHO);
}

uint8_t YKX122::getAddr() const{
  return addr;
}
  


