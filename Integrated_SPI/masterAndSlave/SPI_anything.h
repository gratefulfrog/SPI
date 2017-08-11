#ifndef SPI_anything_h
#define SPI_anything_h

#include <Arduino.h>

const char nullChar = '#';
const int pauseBetweenSends = 20; //us

template <typename T> 
unsigned int SPI_writeAnything (const T& value){
  const byte * p = (const byte*) &value;
  unsigned int i  = 0,
              lim = sizeof value;
  for (; i < lim; i++){
    SPI.transfer(*p++);
    delayMicroseconds(pauseBetweenSends);
  }
  return i;
}

template <typename T> 
unsigned int SPI_readAnything(T& value){
  byte * p = (byte*) &value;
  unsigned int i = 0,
               lim = sizeof value;
  for (; i < lim; i++){
    *p++ = SPI.transfer (nullChar);
    delayMicroseconds (pauseBetweenSends);
  }
  return i;
}
 
template <typename T> 
unsigned int SPI_readAnything_reprime(T& value, byte prime){
  byte * p = (byte*) &value;
  unsigned int i =0,
               limLessOne = (sizeof value) -1;
  for (; i < limLessOne; i++){
    *p++ = SPI.transfer (nullChar);
    delayMicroseconds (pauseBetweenSends);
  }
  *p++ = SPI.transfer (prime);
  delayMicroseconds (pauseBetweenSends);
  return i;
}
 
template <typename T> 
unsigned int SPI_readAnything_ISR(T& value){
  byte * p = (byte*) &value;
  unsigned int i=1,
               lim = sizeof value;
  *p++ = SPDR;  // get first byte
  for (; i < lim; i++){
    *p++ = SPI.transfer (nullChar);
    delayMicroseconds (pauseBetweenSends);
  }
  return i;
} 

#endif
