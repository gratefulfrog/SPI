#ifndef SPI_anything_h
#define SPI_anything_h

//#include <Arduino.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <wiringPiSPI.h>

const int pauseBetweenSends   = 20;  // microseconds
const struct timespec pauseStruct = {
  0,
  pauseBetweenSends*1000
};

const uint8_t nullChar = '#';


template <typename T>
unsigned int SPI_writeAnything (int channel, const T& value) {
  const uint8_t * p = (const uint8_t*) &value;
  unsigned int i;
  for (i = 0; i < sizeof value; i++){
    wiringPiSPIDataRW(channel, p++, 1);
    nanosleep(&pauseStruct,NULL);
    //SPI.transfer(*p++);
    //delay(pauseBetweenSends);
  }
  return i;
} 

template <typename T>
unsigned int SPI_readAnything(int channel, T& value){
  uint8_t * p = (uint8_t*) &value;
  unsigned int i;
  for (i = 0; i < sizeof value; i++){
    *p++ = wiringPiSPIDataRW(channel,&nullChar, 1);
    nanosleep(&pauseStruct,NULL);
    //*p++ = SPI.transfer (0);
  }
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
