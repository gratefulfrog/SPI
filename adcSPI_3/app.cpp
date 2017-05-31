#include "app.h"

void App::instantiateADCs(){
  // AD7689 connected through SPI with SS specified in constructor
  // use default settings (8 channels, unipolar, referenced to 4.096V internal bandga)
 
  for (uint8_t i = 0; i< USARTSPI::nbUARTS; i++){
    if(adc2TestVec[i]){
      YSPI *yyy;
      if (usingUSARTSPI){
        yyy = usartInit(i);
      }
      else{  // HW SPI
        yyy = hwInit(); 
      }
      // step 2: AD7689 instantiation
      flashInfo(2);
      delay(1000);
      *(adcVec+i) = new AD7689(yyy, nbChannels); 
      flash(*(adcVec+i));
      (useSerial && Serial.println(String("AD7689 instance :\t") +String(i) + String(" created!")));
      delay(1000);
      // step 3: AD7689 self-test
      flashInfo(3);
      delay(1000);
      //flash(adcVec[i]->selftest());
      doSelfTest(*(adcVec+i));
    }
  }
}

void  App::updateADCAndChannelCounters(){
  ch_cnt = (ch_cnt + 1) % nbChannels;
  if (!ch_cnt && usingUSARTSPI){
    // we reached the last channel so we go to next adc
    uint8_t i = (currentADC + 1) %USARTSPI::nbUARTS ;
    while(!adc2TestVec[i]){
      i = (i+1) % USARTSPI::nbUARTS ;
    }
    currentADC = i;
  }
}
