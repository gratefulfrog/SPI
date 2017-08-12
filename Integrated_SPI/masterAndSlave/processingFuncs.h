#ifndef PROCESSINGFUNCS_H
#define PROCESSINGFUNCS_H

#include <Arduino.h>
#include "adcMgr.h"

typedef void (*processingUint32FuncPtr)(uint32_t &v);  
typedef void (*processingUintTVSFuncPtr)(timeValStruct_t &v);  


// define functions here for various forms of processing of the data coming from the boards

extern void serialPrintUint32(uint32_t &v);
extern void serialPrintTVS(timeValStruct_t &tvs);


#endif
