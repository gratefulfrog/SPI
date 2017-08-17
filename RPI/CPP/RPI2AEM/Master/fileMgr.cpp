#include "fileMgr.h"

using namespace std;

// protected

const uint8_t FileMgr::BoardID = BOARD_BOARD_ID;
const uint8_t FileMgr::BoardChannelVec[] = {BOARD_BOARD_0_NB_CHANNELS,BOARD_BOARD_1_NB_CHANNELS};
const uint8_t FileMgr::nbBoardChannels = FileMgr::BoardChannelVec[FileMgr::BoardID];

const uint8_t FileMgr::nbHeaders = 4;
const char * const FileMgr::headerVec[] = {HEADER_0, HEADER_1, HEADER_2, HEADER_3};
const char* FileMgr::fileNameMask  = "./DATA/Board:%010u@%s.csv";
const char* FileMgr::timeMask = "%F:%X";

  
bool FileMgr::ok2Create() const{
  return bOK && tOK;
}

void FileMgr::createFilename (){
  time_t rawtime;
  struct tm * timeinfo;
  static const int bufSize = 80,
    fileNameSize = 100;
  char buffer[bufSize];

  csvFilename = new char[fileNameSize];
  
  time ( &rawtime );

  timeinfo = localtime (&rawtime);

  strftime (buffer,80,timeMask,timeinfo);
  sprintf(csvFilename,fileNameMask,bid,buffer);
  //cout<< "csvFilename: " << csvFilename<<endl;
}

void FileMgr::createFile(){
  // it is already ok to create
  createFilename();
  try {
    csvfile csv(csvFilename); //will create or open in append mode
    for(uint8_t i=0;i < nbHeaders ;i++){
      csv << headerVec[i];
    }
    csv << endrow;
    wOK = true;
  }
  catch (const std::exception& ex){
    std::cout << "Exception was thrown: " << ex.what() << std::endl;
  }
}
void FileMgr::putDataElt(csvfile &f, const timeValStruct_t &tvs) const{
  uint8_t l,r;
  decode(tvs.aidcid,l,r);
  f <<  (int)l << (int)r  << tvs.t << tvs.v << endrow;
}

void FileMgr::writeRows(){
  if(!wOK){
    cout << "call to writeRows with wOK == false!" << endl;
    exit (EXIT_FAILURE);;
  }
  cout << "Writing to disk..." << writeCount++ << endl;
  try {
    csvfile csv(csvFilename); //will create or open in append mode
    for(uint8_t i=0;i < maxWrites ;i++){
      putDataElt(csv,writeBufferVec[i]);
    }
    nextWriteIndex=0;
  }
  catch (const std::exception& ex){
    std::cout << "Exception was thrown: " << ex.what() << std::endl;
  }
}
  
FileMgr::FileMgr(uint8_t nbChannels):
  nextWriteIndex(0),
  bOK(false),
  tOK(false),
  wOK(false),
  maxWrites(nbChannels),
  writeCount(0){ 
  
  writeBufferVec = new timeValStruct_t[maxWrites];
}
void FileMgr::setBID(boardID bd){
  bid = bd;
  bOK = true;
  //cout << "setBID !" << endl;
  if (ok2Create()){
    createFile();
  }
}

void FileMgr::setTID(){
  tOK = true;
  //cout << "setTID !" << endl;
  if (ok2Create()){
    createFile();
  }
}
void FileMgr::addTVS(const timeValStruct_t &tvs){
  writeBufferVec[nextWriteIndex++] = tvs;
  if (nextWriteIndex == maxWrites){
    writeRows();
  }
}

  
