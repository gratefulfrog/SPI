#ifndef FILEMGR_H
#define FILEMGR_H

#include <stdint.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <string>

#include "csvfile.h"

#include <stdio.h>      /* printf */
#include <time.h>       /* time_t, struct tm, time, localtime, asctime */

#include "config.h"
#include "utilities.h"



/** one FileMgr instance handles one board */
class FileMgr{
protected:
  static const char **headerVec;
  static const char *fileNameMask;
  boardID     bid;
  timeStamp_t tid;
  bool bOK = false,
    tOK    = false;
  timeValStruct_t *writeBufferVec;  // will contain sum of all values in channel vec elts,
  
  bool ok2Create();
  void createFile();
  void writeRows();
public:
  FileMgr(uint8_t nbADC, uint8_t *channelVec);
  setBID(boardID bd);
  setTID(timeStamp_t tid);
  addTVS(timeValStruct_t &tvs);
};
  
#endif
