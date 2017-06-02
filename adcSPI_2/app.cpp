#include "app.h"


void App::print(String s){
  (useSerial    && 
   Serial.print(s));
}
void App::println(String s){
  print(s + String("\n"));
}

App::App(uint8_t id) : adcID(id){
}
