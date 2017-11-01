#include <stdio.h>

#include "I2C.h"
#include "k122.h"

byte addrVec[] = {0x1E,0x1F},
     whoAmI = 0x0F;

String lables[] = {"\tX : ","\tY : ","\tZ : "};

const byte XOUT_L = 0x06,
           XOUT_H = 0x07,
           YOUT_L = 0x08,
           YOUT_H = 0x09,
           ZOUT_L = 0x0A,
           ZOUT_H = 0x0B;
           
const byte regVec[][2] = {{XOUT_L,XOUT_H},
                          {YOUT_L,YOUT_H},
                          {ZOUT_L,ZOUT_H}};

#define K122_2G       (0x00)
#define K122_4G       (0x08)
#define K122_8G       (0x10)
#define K122_LOW_RES  (0x00)
#define K122_HI_RES   (0x40)
#define K122_OPERATE  (0x80)

#define K122_DR_12_5  (0x00)
#define K122_DR_25    (0x01)
#define K122_DR_50    (0x02)
#define K122_DR_100   (0x03)
#define K122_DR_200   (0x04)
#define K122_DR_400   (0x05)
#define K122_DR_800   (0x06)
#define K122_DR_1600  (0x07)
#define K122_DR_0_781 (0x08)
#define K122_DR_1_563 (0x09)
#define K122_DR_3_125 (0x0A)
#define K122_DR_6_25  (0x0B)
#define K122_DR_3200  (0x0C)
#define K122_DR_6400  (0x0D)
#define K122_DR_12800 (0x0E)
#define K122_DR_25600 (0x0F)

#define CNTL1         (0x18) // main features control registger, need to set in 2 phases, first without operational mode, 
                             // then set operational
#define ODCNTL        (0x1B) // output data rate control register


void configK122(){
  /*
  byte regVal[][2] = {{0x18,0x50}, // set to high res and +/-8g mode //  0x40 =>  high res 2G, 48 = high res, 4G,
                      {0x1B,0x02}, // set output data rate 
                      {0x18,0xC0}};  //PC1 = operational,  RES = high res
  */
  byte regVal[][2] = {{CNTL1,  K122_HI_RES | K122_8G},        // set to high res and +/-8g mode
                      {ODCNTL, K122_DR_1600},                 // set output data rate 1600Hz
                      {CNTL1,  K122_OPERATE}};                // set operational mode
  
  for (int j=0;j<2;j++){
    for(int i=0;i<3;i++){
      I2c.write(addrVec[j],regVal[i][0],regVal[i][1]);
    }
  }
}

int getAccChannelValue(byte accID, byte chanID){
  I2c.write(addrVec[accID], regVec[chanID][0]);
  uint8_t bytes[2];
  for (byte i=0;i<2;i++){
    I2c.read(addrVec[accID], regVec[chanID][i],1,&bytes[i]);
  }
  return ((bytes[1]<<8)|bytes[0]);
}


void setup(){
  Serial.begin(1000000);  // start serial port at 9600 bps
  while (!Serial) {}
    
  I2c.begin();
  
  for (int i=0;i<2;i++){
    I2c.write(addrVec[i],whoAmI);
    I2c.read(addrVec[i],1);
    while (!I2c.available());
    char buff[24],
         templ[] = "0x%X : whom am I : 0x%X";
    sprintf(buff,templ,addrVec[i],I2c.receive());
    Serial.println(buff);
    // response should be 0b11011 = 0x1B
  }
  delay(1000);
  configK122();
}

void loop(){
  for (int i=0;i<2;i++){
    char buffer[14];
    sprintf(buffer,"\t0x%X : ", addrVec[i]);
    Serial.print(buffer);
    for (int j=0;j<3;j++){
      Serial.print(lables[j] + String(getAccChannelValue(i,j)));
    }
  }
  Serial.println();
  delay(200);
}

