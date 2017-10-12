#include "app.h"

// encapsualte the test for useSerial boolean
void App::print(String s) const{
  (useSerial    && 
   Serial.print(s));
}
// add a \n to the string and call the print, note that virtuality could mean that a local print method is called instead of App:print
void App::println(String s) const{
  print(s + String("\n"));
}

// constrcutor , init the adcID
App::App(uint8_t id) : adcID(id){
}
