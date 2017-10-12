#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>
#include <time.h>

const uint8_t nullChar =  '#';

extern void  delay(time_t secs);
extern void  delayMicroseconds(long microsecs);

#endif
