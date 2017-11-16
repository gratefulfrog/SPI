#include "guid_24AA025UID.h"

AA025UID::AA025UID(): I2C_addr(GUID_I2C_ADR), guidID(0){

  Wire.beginTransmission(I2C_addr);
  Wire.write(AA025UID_GUID_ADDR);
  Wire.endTransmission();    
  
  Wire.requestFrom(I2C_addr, GUID_ID_LEN);
  while (!Wire.available());
  
  uint8_t guidIDVec[GUID_ID_LEN];

  uint8_t i = 0;
  for (uint8_t i = 0;((i<GUID_ID_LEN) && Wire.available());i++){
    guidIDVec[i] = Wire.read();
  }

  for (uint8_t j=GUID_SERIAL_NB_START;j<GUID_ID_LEN;j++){
    guidID = (guidID<<8) | guidIDVec[j];
  }
}

const uint32_t& AA025UID::getGuidID() const{
  return guidID;
}


/*
 AA025UID::AA025UID(): I2C_addr(GUID_I2C_ADR), guidID(0){

  I2c.write(I2C_addr, AA025UID_GUID_ADDR);
  I2c.read(I2C_addr, GUID_ID_LEN);

  uint8_t guidIDVec[GUID_ID_LEN];

  uint8_t i = 0;
  for (uint8_t i = 0;((i<GUID_ID_LEN) && I2c.available());i++){
    guidIDVec[i] = I2c.receive();
  }

  for (uint8_t j=GUID_SERIAL_NB_START;j<GUID_ID_LEN;j++){
    guidID = (guidID<<8) | guidIDVec[j];
  }
}

 */
