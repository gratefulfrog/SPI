
#include "Arduino.h"

#include <stdio.h>

#include "I2C.h"

void setupDevice(uint8_t adr){
  // disable device to update Configuration
  I2c.write(adr, 0x18, 0);
  
  // set data output frequency and filter cut-off frequency
  uint8_t IIR_BYPASS = 0x80;
  uint8_t LPRO = 0x40;
  uint8_t OSA3 = 0x08, OSA2 = 0x04, OSA1 = 0x02, OSA0 = 0x01;
  uint8_t ODCNTL = IIR_BYPASS | LPRO | OSA2 | OSA1 | OSA0; // set filter to 800 Hz and output data rate to 1.6 kHz
  I2c.write(adr, 0x1B, ODCNTL);
  
  // set resolution and enable device
  uint8_t xPC1 = 0x80; // device enable
  uint8_t RES = 0x40; // high resolution enable
  uint8_t DRDYE = 0x20; // interrupt enable
  uint8_t GSEL1 = 0x10, GSEL0 = 0x08; // 2g/4g/8g range
  uint8_t TDTE = 0x04; // directional tap enable
  uint8_t WUFE = 0x02; // motion detect wake up enable
  uint8_t TPE = 0x01; // tilt position enable
  
  uint8_t conf = RES;
  Serial.println("conf = " + String(conf, BIN));
  I2c.write(adr, 0x18, conf);
  
  I2c.write(adr, 0x18, xPC1);
}

const uint8_t aVec[] = {0x1E, 0x1F};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Up and running!");
  
  I2c.begin();
}

void showDevice(uint8_t adr){

  I2c.write(adr, 0x06);
  
  uint8_t values[6];
  I2c.read(adr, 6, &values[0]);
  
  uint8_t KX122_Accel_X_LB = values[0];
  uint8_t KX122_Accel_X_HB = values[1];
  uint8_t KX122_Accel_Y_LB = values[2];
  uint8_t KX122_Accel_Y_HB = values[3];
  uint8_t KX122_Accel_Z_LB = values[4];
  uint8_t KX122_Accel_Z_HB = values[5];

  int16_t KX122_Accel_X_RawOUT = (KX122_Accel_X_HB<<8) | (KX122_Accel_X_LB);
  int16_t KX122_Accel_Y_RawOUT = (KX122_Accel_Y_HB<<8) | (KX122_Accel_Y_LB);
  int16_t KX122_Accel_Z_RawOUT = (KX122_Accel_Z_HB<<8) | (KX122_Accel_Z_LB);
  
  float KX122_Accel_X_OUT = (float)KX122_Accel_X_RawOUT / 16384.0; // 16384
  float KX122_Accel_Y_OUT = (float)KX122_Accel_Y_RawOUT / 16384.0;
  float KX122_Accel_Z_OUT = (float)KX122_Accel_Z_RawOUT / 16384.0;

  char buf[20],
       templ[] = "Device: %X\tX: ";
  sprintf(buf,templ, adr);
  Serial.print(buf);
  Serial.print(KX122_Accel_X_OUT,3);
  Serial.print("\tY: ");
  Serial.print(KX122_Accel_Y_OUT,3);
  Serial.print("\tZ: ");
}

void loop() {
  for(uint8_t i=0; i<2;i++){
    showDevice(aVec[i]);
    Serial.print("\t\t");
  }
  Serial.println();
  delay(200);
}
