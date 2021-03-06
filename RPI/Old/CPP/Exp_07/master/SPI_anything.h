#ifndef SPI_anything_h
#define SPI_anything_h

#include <stdint.h>
#include <unistd.h>

#include "utilities.h"
#include "spi.h"

const char nullChar         = '#'; // SPI_A_NULL_CHAR;
const int pauseBetweenSends = 5; // 10 works // SPI_A_PAUSE_BETWEEN_SENDS; 


template <typename T>
unsigned int SPI_writeAnything (SPI &spi, const T& value) {
  const uint8_t * p = (const uint8_t*) &value;
  unsigned int i  = 0,
    lim = (sizeof value);
  for (; i < lim; i++){
    spi.transfer(*p++);
    delayMicroseconds(pauseBetweenSends);
  }
  return i;
}


template <typename T>
unsigned int SPI_readAnything(SPI &spi,T& value){
  uint8_t * p = (uint8_t*) &value;
  unsigned int i = 0,
    lim = sizeof (value);
  for (; i < lim; i++){
    *p++ = spi.transfer (nullChar);
    delayMicroseconds (pauseBetweenSends);
  }
  return i;
}

template <typename T> 
unsigned int SPI_readAnything_reprime(SPI &spi, T& value, const uint8_t prime){
  uint8_t * p = (uint8_t*) &value;
  unsigned int i =0,
               limLessOne = (sizeof value) -1;
  for (; i < limLessOne; i++){
    *p++ = spi.transfer (nullChar);
    delayMicroseconds (pauseBetweenSends);
  }
  *p = spi.transfer (prime);
  delayMicroseconds (pauseBetweenSends);
  return i;
}
  
template <typename T> unsigned int SPI_readAnything_ISR(SPI &spi,T& value){
  uint8_t * p = (uint8_t*) &value;
  unsigned int i =1,
    lim = sizeof(value);
  *p++ = spi.getSPDR();  // get first uint8_t

  for (; i < lim; i++){
    *p++ = spi.transfer (nullChar);
    delayMicroseconds (pauseBetweenSends);
  }
  return i;
}  


#endif
