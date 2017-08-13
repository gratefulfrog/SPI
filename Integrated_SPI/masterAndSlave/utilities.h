#ifndef UTILITIES_H
#define UTILITIES_H

#include <Arduino.h>

typedef uint32_t timeStamp_t; /**< typedef for timestamps */

typedef float   ADC_value_t; /**< typedef for ADC values*/
typedef uint8_t ADCCID_t;    /**< typedef for ADC AID and CID encoded onto 8 bits */

/** timeValStruct_t definition. This struct holds:
 *  aidcid: an encoded value with ADC id and Channel Id encoded as AAAACCCC on 8 bits, the value 255 special and indcates a NULL struct to be ignored
 *  
 *  t: is the timestamp value of the data
 *  
 *  v: is the reading from the ADC channel */
typedef struct timeValStruct_t {
  ADCCID_t      aidcid;
  timeStamp_t   t;
  ADC_value_t   v;
};

typedef void (*processingUint32FuncPtr)(uint32_t &v);         /**< typedef for funciton pointer to a function that will process a uint32_t value*/
typedef void (*processingUintTVSFuncPtr)(timeValStruct_t &v); /**< typedef for funciton pointer to a function that will process a timeValStruct_t */


// define functions here for various forms of processing of the data coming from the boards

extern void serialPrintUint32(uint32_t &v);
extern void serialPrintTVS(timeValStruct_t &tvs);

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
