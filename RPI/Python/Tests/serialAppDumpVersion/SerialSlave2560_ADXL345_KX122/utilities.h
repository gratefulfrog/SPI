#ifndef UTILITIES_H
#define UTILITIES_H

#include <Arduino.h>

#include "config.h"



// define functions here for various forms of processing of the data coming from the boards

//extern void serialPrintUint32(uint32_t &v);
//extern void serialPrintTVS(timeValStruct_t &tvs);

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
