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
  static const uint8_t nbHeaders;        /*!< nb of headers to write to CSV data file */
  static const char * const headerVec[]; /*!< array of headers to write to CSV data file */
  static const char *fileNameMask;       /*!< printf formated string used to generate the filename with board id and time string */
  static const char* timeMask;           /*!< printf formated string used to generate the time string for filename with current time and date */
  boardID     bid;                       /*!< board id to be used with printf formated string used to generate the filename */ 
  uint8_t nextWriteIndex,                /*!< index for writeBuffer vec, where next TVS will be put */
    maxWrites;                           /*!< size of write buffer, i.e. sum of number of ADC channels for the board */
  bool bOK,                              /*!< TRUE if the board ID has been received, used to know when to create the CSV file */
    tOK,                                 /*!< TRUE if the initial timestamp has been received, used to know when to create the CSV file */
    wOK;                                 /*!< TRUE if it is ok to write records to the csv file, i.e. the file has been created and headers written. */
  timeValStruct_t *writeBufferVec;       /*!< tvs structs are stored here until they are all available, then they are written to disk and nextWriteIndex is reset */

  uint32_t writeCount;                   /*!< keeps track of how many disk writes have been done, used to inform the user */
  
  char* csvFilename;                     /*!< the name of the csv file, used for writing */
  
  bool ok2Create() const;                /*!< returns TRUE if it is ok to create the csv file */
  unisgned in badData;                   /*!< keeps count of bad data elts received from masterApp */
  void createFilename ();                /*!< creates the filename and writes it to csvFilename (allocates memory)  */
  void createFile();                     /*!< creates the csv file and writes the header row */
  void putDataElt(csvfile &f, const timeValStruct_t &tvs) const;  /*!< puts a tvs into the writeBufferVec and if it's time to write to disk calls writeRows()  */
  void writeRows();                     /*!< writes the content of writeBufferVec to the csv file and resets the nextWriteIndex  */
  /** check the validitiy of a timeValStruct, currently only for testing on ADC 1, 8 Channels
   * @param &tvs the const tvs to check
   * @return true if ok, false if not, stdout gets a message if not ok */
  bool isValid(const const timeValStruct_t &tvs) const;
public:
  static const uint8_t BoardID;           /*!< configuration parameter for the application, used to determine number of board channels */
  static const uint8_t BoardChannelVec[]; /*!< configuration parameter for the application, contains nb of channels for the boardID index */
  static const uint8_t nbBoardChannels;   /*!< configuration parameter for the application, contains nb of board channels for the application */

  /** instance constructor, initializes all values
   * @param nbChannels is the total number of adc channels on the board, ie the sum of the nb of channels on each adc of the board */
  FileMgr(uint8_t nbChannels);
  /** sets the Board id and bOK
   * @param &bid the board id */
  void setBID(const uint32_t &bd);
  /** called to set the time0  */
  void setTID();
  /** adds a tvs to the writeBufferVec and writes the entire buffer to disk when full
   * @param &tvs a ref to a const tvs that will be COPIED to the write buffer */
  void addTVS(const timeValStruct_t &tvs);
};

typedef void (FileMgr::*diskProcessingUint32FuncPtr)(const uint32_t &bd); /*!< typedef for funciton pointer to a function that will write to disk uint32_t value*/
typedef void (FileMgr::*diskProcessingVoidFuncPtr)(); /*!< typedef for funciton pointer to a function that will write to disk void  value*/
typedef void (FileMgr::*diskProcessingTVSFuncPtr)(const timeValStruct_t &v); /*!< typedef for funciton pointer to a function that will write to disk  a timeValStruct_t */



#endif
