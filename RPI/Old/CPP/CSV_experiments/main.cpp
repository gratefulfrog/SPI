
#include <stdint.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <string>

#include "csvfile.h"

#include <stdio.h>      /* printf */
#include <time.h>       /* time_t, struct tm, time, localtime, asctime */

typedef uint32_t timeStamp_t; /*!< typedef for timestamps */
typedef float   ADC_value_t; /*!< typedef for ADC values*/
typedef uint8_t ADCCID_t;    /*!< typedef for ADC AID and CID encoded onto 8 bits */

struct timeValStruct_t {
  ADCCID_t      aidcid;
  timeStamp_t   t;
  ADC_value_t   v;
} __attribute__((__packed__));

// decodes bits LLLLRRRR to LLLL, RRRR , max value to decode is 255
void decode(const uint8_t &coded, uint8_t &left, uint8_t &right){
  left = (coded >> 4) & 0xF;
  right = coded & 0xF;
}

void putDataElt(csvfile &f, const timeValStruct_t &tvs){
  uint8_t l,r;
  decode(tvs.aidcid,l,r);
  f <<  (int)l << (int)r  << tvs.t << tvs.v << endrow;
}

char* getFilename (uint32_t boardID){
  time_t rawtime;
  struct tm * timeinfo;
  static char filename[100];
  
  time ( &rawtime );
  char buffer [80];

  timeinfo = localtime (&rawtime);

  strftime (buffer,80,"%F:%X",timeinfo);
  sprintf(filename,"Board:%032d@%s.csv",boardID,buffer);
  puts (filename);
  return filename;
}

bool exists(const char *fileName){
    std::ifstream infile(fileName);
    bool res = infile.good();
    infile.close();
    return res;
}
inline bool exists_test3 (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

int main(){
  const int tSize = 3;
  const timeValStruct_t tvsVec[tSize] = {{1<<4,0,0.01},
					 {1<<4|1,1,1.02},
					 {3<<4|2,2,2.03}};
  const char* filename = "frank.csv";
  bool headers = !exists_test3(filename);
  
  try  {
    //csvfile csv(getFilename(27)); // throws exceptions!
    csvfile csv("frank.csv"); // throws exceptions!    
    
    // Header
    if(headers){
      csv << "ADC" << "Channel"<< "TimeStamp" << "Value" << endrow;
    }
    // Data
    for (int i=0;i<tSize;i++){
      putDataElt(csv,tvsVec[i]);
    }
  }
  catch (const std::exception& ex){
    std::cout << "Exception was thrown: " << ex.what() << std::endl;
  }
  return 0;
}
