#ifndef SPI_anything_h
#define SPI_anything_h

#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <wiringPiSPI.h>

#include "config.h"


const char nullChar =         SPI_A_NULL_CHAR;
const int pauseBetweenSends = SPI_A_PAUSE_BETWEEN_SENDS; //20; //us

const struct timespec pauseStruct = {
  0,
  pauseBetweenSends*1000
};


template <typename T>
unsigned int SPI_writeAnything (int channel, const T& value) {
  const uint8_t * p = (const uint8_t*) &value;
  unsigned int i;
  for (i = 0; i < sizeof value; i++){
    wiringPiSPIDataRW(channel, p++, 1);
    nanosleep(&pauseStruct,NULL);
  }
  return i;
} 

template <typename T>
unsigned int SPI_readAnything(int channel, T& value){
  uint8_t * p = (uint8_t*) &value;
  unsigned int i;
  for (i = 0; i < sizeof value; i++){
    wiringPiSPIDataRW(channel,p++, 1);
    nanosleep(&pauseStruct,NULL);
  }
  return i;
}

template <typename T> 
unsigned int SPI_readAnything_reprime(int channel, T& value, uint8_t prime){
  uint8_t * p = (uint8_t*) &value;
  unsigned int i =0,
               limLessOne = (sizeof value) -1;
  for (; i < limLessOne; i++){
    wiringPiSPIDataRW(channel,p++, 1);
    nanosleep(&pauseStruct,NULL);
    
  }
  *p = prime;
  wiringPiSPIDataRW(channel,p++, 1);
  nanosleep(&pauseStruct,NULL);
  return i;
}

  
/* This one seems unfeasible on rpi;
 * it is only used when in slave mode so let's forget it!  
template <typename T> unsigned int SPI_readAnything_ISR(T& value){
  uint8_t * p = (uint8_t*) &value;
  unsigned int i;
  *p++ = SPDR;  // get first uint8_t

  for (i = 1; i < sizeof value; i++)
    *p++ = SPI.transfer (0);
  return i;
}  
*/

#endif
