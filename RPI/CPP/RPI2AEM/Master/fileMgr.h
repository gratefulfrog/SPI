#ifndef FILEMGR_H
#define FILEMGR_H

#include <stdint.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>

#include <stdio.h>      /* printf */
#include <time.h>       /* time_t, struct tm, time, localtime, asctime */

#include "csvfile.h"
#include "config.h"
#include "utilities.h"


/** one FileMgr instance handles one board */
class FileMgr{
protected:
  static const uint8_t nbHeaders;
  static const char * const headerVec[];
  static const char *fileNameMask;
  static const char* timeMask;
  boardID     bid;
  uint8_t nextWriteIndex,
    maxWrites;  // init in constuctor by sum of all nb channels
  bool bOK,
    tOK,
    wOK;
  timeValStruct_t *writeBufferVec;  // will contain sum of all values in channel vec elts,

  uint32_t writeCount;
  
  char* csvFilename;
  
  bool ok2Create() const;
  void createFilename ();
  void createFile();
  void putDataElt(csvfile &f, const timeValStruct_t &tvs) const;
  void writeRows();
public:
  static const uint8_t BoardID;
  static const uint8_t BoardChannelVec[];
  static const uint8_t nbBoardChannels;

  FileMgr(uint8_t nbChannels); // total nb of channels , ie the sum of the nb of channels on each adc of the board
  void setBID(const uint32_t &bd);
  void setTID();
  void addTVS(const timeValStruct_t &tvs);
};

typedef void (FileMgr::*diskProcessingUint32FuncPtr)(const uint32_t &bd); /*!< typedef for funciton pointer to a function that will write to disk uint32_t value*/
typedef void (FileMgr::*diskProcessingVoidFuncPtr)(); /*!< typedef for funciton pointer to a function that will write to disk void  value*/
typedef void (FileMgr::*diskProcessingTVSFuncPtr)(const timeValStruct_t &v); /*!< typedef for funciton pointer to a function that will write to disk  a timeValStruct_t */



#endif
