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
  cout << "Writing to disk... " << writeCount++ << " Bad data elts: " << badData << endl;
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

bool FileMgr::isValid(const timeValStruct_t &tvs) const{
  static timeStamp_t lastTimeStamp= tvs.t;
  const static timeStamp_t  maxOverFlowTimeStamp = 500000;
  uint8_t aid,cid;
  decode(tvs.aidcid,aid,cid);
  bool  ok = true;
  if (aid !=1){
    cout << "  bad ADC ID: " << (int)aid  << endl;
    ok = false;
  }
  else if (cid >7){
    cout << "  bad Channel ID: " << (int)cid  << endl;
    ok = false;
  }
  else if ((tvs.t < lastTimeStamp) && (tvs.t > maxOverFlowTimeStamp)){
    cout << "  bad TimeStamp: " << tvs.t  << endl;
    ok = false;
  }
  else if ((tvs.v > 0.0 && tvs.v <0.000001) || (tvs.v > 3.5)){
    cout << "  bad Value: " << tvs.v << endl ;
    ok =false;
  }
  return ok;
}

FileMgr::FileMgr(uint8_t nbChannels):
  nextWriteIndex(0),
  bOK(false),
  tOK(false),
  wOK(false),
  maxWrites(nbChannels),
  writeCount(0),
  badData(0){ 
  
  writeBufferVec = new timeValStruct_t[maxWrites];
  cout << "init file mgr!" << endl;
}
void FileMgr::setBID(const uint32_t &bd){
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
    
void FileMgr::addTVS(const timeValStruct_t tvs){
  //cout << "writing a tvs" << endl;
  if (isValid (tvs)){
    writeBufferVec[nextWriteIndex++] = tvs;
    if (nextWriteIndex == maxWrites){
      writeRows();
    }
  }
  else{
    badData++;
  }
}

  
