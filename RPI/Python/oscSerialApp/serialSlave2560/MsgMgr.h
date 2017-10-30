#ifndef MSGGR_H
#define MSGGR_H

#include <Arduino.h>

#include <OSCMessage.h>
#include <SLIPEncodedSerial.h>

#include "config.h"

class MsgMgr{
  protected:
    SLIPEncodedSerial *SLIPSerial;

  public:
    MsgMgr();
    void send(u8u32f_struct &t);
    void send(uint32_t i);

};

#endif
  
