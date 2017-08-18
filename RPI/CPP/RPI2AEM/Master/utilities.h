#ifndef UTILITIES_H
#define UTILITIES_H

#include "config.h"


//class FileMgr;

// define functions here for various forms of processing of the data coming from the boards


extern void delay(time_t millisecs);            /*!< emulates Arduino 'delay' function, but will not work if arg is too big!, use 'delaySeconds' instead */
extern void delayMicroseconds(long microsecs);  /*!< emulates Arduino 'delayMicroseconds' function */
extern void delaySeconds(time_t secs);          /*!< emulates Arduino 'delay' function, but for seconds not millis */
extern void delayFull(time_t secs, long micros); /*!< encapsulates the call to nanosleep, with seconds and micro-seconds */

extern void serialPrintUint32(const uint32_t &v);       /*!< emulates serial printing of a uint32, ie print to stdout */
extern void serialPrintTVS(const timeValStruct_t &tvs); /*!< emulates serial printing of a tvs, ie print to stdout */

/** encodes bits LLLL, RRRR to LLLLRRRR , max value to encode is 15
 *  @param &coded : a reference the encoded value that the function will create 
 *  @param &left  : read only value for the left 4 bits
 *  @param &right : read only value for the right 4 bits */
extern void encode(uint8_t &coded, const uint8_t &left, const uint8_t &right);

/** decodes bits LLLLRRRR to LLLL, RRRR , max value to decode is 255
 *  @param &coded : a read only value containing the coded bits
 *  @param &left  : reference to variable for the left 4 decoded bits 
 *  @param &right : reference to variable for the right 4 decoded bits  */
extern void decode(const uint8_t &coded, uint8_t &left, uint8_t &right);

#endif
