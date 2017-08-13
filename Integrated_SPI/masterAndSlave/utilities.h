#ifndef UTILITIES_H
#define UTILITIES_H

#include <Arduino.h>

typedef uint32_t timeStamp_t;


typedef float   ADC_value_t;
//typedef uint8_t channelID_t;
typedef uint8_t ADCCID_t;

typedef struct timeValStruct_t {
  ADCCID_t      aidcid;
  timeStamp_t   t;
  ADC_value_t   v;
};

typedef void (*processingUint32FuncPtr)(uint32_t &v);  
typedef void (*processingUintTVSFuncPtr)(timeValStruct_t &v);  


// define functions here for various forms of processing of the data coming from the boards

extern void serialPrintUint32(uint32_t &v);
extern void serialPrintTVS(timeValStruct_t &tvs);

// encodes bits LLLL, RRRR to LLLLRRRR , max value to encode is 15
extern void encode(uint8_t &coded, const uint8_t &left, const uint8_t &right);

// decodes bits LLLLRRRR to LLLL, RRRR , max value to decode is 255
extern void decode(const uint8_t &coded, uint8_t &left, uint8_t &right);

#endif
