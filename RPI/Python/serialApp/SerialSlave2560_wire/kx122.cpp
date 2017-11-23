#include "kx122.h"

const uint8_t YKX122::addrVec[] = {0x1E,0x1F};
const uint8_t YKX122::regVec[][2] = {{KX122_XOUT_L,KX122_XOUT_H},
				                             {KX122_YOUT_L,KX122_YOUT_H},
				                             {KX122_ZOUT_L,KX122_ZOUT_H}};

void YKX122::configKX122() const{
  uint8_t regVal[][2] = {{KX122_ODCNTL, KX122_IIR_BYPASS | KX122_LPRO | KX122_OSA2 | KX122_OSA1 | KX122_OSA0},
                         {KX122_CNTL1, KX122_HI_RES},        // set HI Res
                         {KX122_CNTL1, KX122_OPERATE}};                // set operational mode
  // disable device for setup
  //I2c.write(addr, KX122_CNTL1,0);
  Wire.beginTransmission(addr);         // transmit to device 
  Wire.write((uint8_t)(KX122_CNTL1));     // sends one byte
  Wire.write((uint8_t)0);                        // sends one byte  
  Wire.endTransmission();  
  
  for (int i=0;i<3;i++){
    //I2c.write(addr,regVal[i][0],regVal[i][1]);
    Wire.beginTransmission(addr); // transmit to device at addr
    Wire.write(regVal[i][0]); 
    Wire.write(regVal[i][1]);
    Wire.endTransmission();  
  }
 }

YKX122::YKX122(uint8_t address, const YSPI *const y)  : YADC(y),addr(address){
  configKX122();
}

float YKX122::acquireChannel(uint8_t channel) {
  //I2c.write(addr, 0x06);
  Wire.beginTransmission(addr); 
  Wire.write((uint8_t)(0x06));        
  Wire.endTransmission();            

  uint8_t bytes[6];
  //I2c.read(addr, 6, &bytes[0]);
  Wire.requestFrom(addr, (uint8_t)6);    // request 6 bytes 

  while(!Wire.available());

  for (uint8_t i=0; ((i<6) && Wire.available());i++){
    bytes[i] = Wire.read();
  }

  return ((bytes[channel*2+1]<<8)|bytes[channel*2])/ 16384.0;
}

bool YKX122::selftest(void) {
  Wire.beginTransmission(addr); 
  Wire.write(KX122_WHOAMI);        
  Wire.endTransmission();            
  Wire.requestFrom(addr, 1);    // request 1 byte

  while(!Wire.available());
  return (Wire.read() == KX122_I_AM_WHO);
}
  
    //I2c.write(addr,KX122_WHOAMI);
    //I2c.read(addr,1);
    //while (!I2c.available());
    // return (I2c.receive() == KX122_I_AM_WHO);

uint8_t YKX122::getAddr() const{
  return addr;
}
