#include "adxl345.h"

void YADXL345::powerOn() const{
  //ADXL345 TURN ON
  writeToSPI(ADXL345_POWER_CTL, 0);  // Wakeup     
  writeToSPI(ADXL345_POWER_CTL, 16); // Auto_Sleep
  writeToSPI(ADXL345_POWER_CTL, 8);  // Measure
}

/* Point to Destination; Write Value; Turn Off  */
void YADXL345::writeToSPI(byte __reg_address, byte __val) const {
  yspi->beginTransaction();
  delayMicroseconds(YADXL345_T_DELAY);
  yspi->transfer(__reg_address); 
  yspi->transfer(__val);
  delayMicroseconds(YADXL345_T_QUIET);
  yspi->endTransaction();
}

void YADXL345::readFromSPI(byte __reg_address, int num, byte _buff[]) const{
  // Read: Most Sig Bit of Reg Address Set
  char _address = 0x80 | __reg_address;
  // If Multi-Byte Read: Bit 6 Set 
  if(num > 1) {
  	_address = _address | 0x40;
  }

  yspi->beginTransaction();
  delayMicroseconds(YADXL345_T_DELAY);

  yspi->transfer(_address);		// Transfer Starting Reg Address To Be Read  
  for(int i=0; i<num; i++){
    _buff[i] = yspi->transfer(0x00);
  }
  
  delayMicroseconds(YADXL345_T_QUIET);
  yspi->endTransaction();
}

void YADXL345::setRegisterBit(byte regAdress, int bitPos, bool state) const {
  byte _b;
  readFromSPI(regAdress, 1, &_b);
  if (state) {
    _b |= (1 << bitPos);  // Forces nth Bit of _b to 1. Other Bits Unchanged.  
  } 
  else {
    _b &= ~(1 << bitPos); // Forces nth Bit of _b to 0. Other Bits Unchanged.
  }
  writeToSPI(regAdress, _b);  
}

void YADXL345::setSpiBit(bool spiBit) const{
  setRegisterBit(ADXL345_DATA_FORMAT, 6, spiBit);
}

void YADXL345::setRangeSetting(int val) const{
  byte _s;
  byte _b;
  
  switch (val) {
  case 2:  
    _s = B00000000; 
    break;
  case 4:  
    _s = B00000001; 
    break;
  case 8:  
    _s = B00000010; 
    break;
  case 16: 
    _s = B00000011; 
    break;
  default: 
    _s = B00000000;
  }
  readFromSPI(ADXL345_DATA_FORMAT, 1, &_b);
  _s |= (_b & B11101100);
  writeToSPI(ADXL345_DATA_FORMAT, _s);
}

void YADXL345::readAccel(int* x, int* y, int* z){
  readFromSPI(ADXL345_DATAX0, ADXL345_TO_READ, _buff);	// Read Accel Data from ADXL345
  
  // Each Axis @ All g Ranges: 10 Bit Resolution (2 Bytes)
  *x = (((int)_buff[1]) << 8) | _buff[0];   
  *y = (((int)_buff[3]) << 8) | _buff[2];
  *z = (((int)_buff[5]) << 8) | _buff[4];
}


YADXL345::YADXL345(const YSPI *const y) : YADC(y){
  // SPI uses data mode 3
  // this is taken care of in the USART settings 

  // values from Sparkfun..
  gains[0] = 0.00376390;
  gains[1] = 0.00376009;
  gains[2] = 0.00349265;

  yspi->setSS(HIGH);  // from the Sparkfun driver code
  powerOn();
  setRangeSetting(16);    // Give the range settings
                          // Accepted values are 2g, 4g, 8g or 16g
                          // Higher Values = Wider Measurement Range
                          // Lower Values = Greater Sensitivity

  setSpiBit(0);           // Configure the device to be in 4 wire SPI mode when set to '0' or 3 wire SPI mode when set to 1
                          // Default: Set to 1
}


float YADXL345::acquireChannel(uint8_t channel){   /*! 0=x, 1=y, 2=z : see getEvent for multipliers !!! */
  int x,y,z;   
  readAccel(&x, &y, &z);  // Read the accelerometer values and store them in variables declared above x,y,z
  float res;
  switch(channel){
  case 0:
    res = x;
    break;
  case 1:
    res = y;
    break;
  case 2:
    res = z;
    break;
  }
  return res*gains[channel];
}

bool YADXL345::selftest(void){
  int x,y,z;   
  delay(100);
  readAccel(&x, &y, &z);  // Read the accelerometer values and store them in variables declared above x,y,z

  // if a value is not zero it's ok!
  bool res = x || y || z;
  if (!res){
    selfTestFailed();
  }
  return true;
}


