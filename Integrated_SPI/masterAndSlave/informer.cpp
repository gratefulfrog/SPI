#include <Arduino.h>
#include "informer.h"

Informer::Informer(uint8_t p) : pin(p){
  pinMode(pin,OUTPUT);
  digitalWrite(pin,LOW);
}

void Informer::informForever (uint8_t nbFlashes) const{
  while (true){
    informOnce(nbFlashes);
  }
  delay(pauseTime);
}

void Informer::informOnce (uint8_t nbFlashes) const {
  for (uint8_t i = 0; i< nbFlashes*2;i++){
    digitalWrite(pin,!digitalRead(pin));
    delay(flashTime);
  }
}

