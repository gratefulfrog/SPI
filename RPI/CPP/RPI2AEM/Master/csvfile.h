#pragma once
#include <string>
#include <iostream>
#include <fstream>


/** Class to manage csv files.
 * this file was taken from rudolfovich's gist
 * https://gist.github.com/rudolfovich/f250900f1a833e715260a66c87369d15 
 * and the close method was added.
 */
class csvfile;

inline static csvfile& endrow(csvfile& file);
inline static csvfile& flush(csvfile& file);

class csvfile{
  std::ofstream fs_;
  const std::string separator_;
  
 public:
 csvfile(const char *filename,
	 const std::string separator = ",",
	 std::ios::openmode mode = std::fstream::out | std::fstream::app):
  fs_(),separator_(separator){
    fs_.exceptions(std::ios::failbit | std::ios::badbit);
    fs_.open(filename,mode);
  }
  
  ~csvfile(){
    flush();
    fs_.close();
  }
  
  void flush(){
    fs_.flush();
  }

  void close(){
    flush();
    fs_.close();
  }

  void endrow()	{
    fs_ << std::endl;
  }
  
  csvfile& operator << ( csvfile& (* val)(csvfile&)){
    return val(*this);
  }
  
  csvfile& operator << (const char * val){
    fs_ << '"' << val << '"' << separator_;
    return *this;
  }
  
  csvfile& operator << (const std::string & val){
    fs_ << '"' << val << '"' << separator_;
    return *this;
  }
  
  template<typename T>
    csvfile& operator << (const T& val){
    fs_ << val << separator_;
    return *this;
  }
};


inline static csvfile& endrow(csvfile& file){
  file.endrow();
  return file;
}

inline static csvfile& flush(csvfile& file){
  file.flush();
  return file;
}

