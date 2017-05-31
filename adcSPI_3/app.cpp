#include "app.h"

void App::instantiateADCs(){
  // AD7689 connected through SPI with SS specified in constructor
  // use default settings (8 channels, unipolar, referenced to 4.096V internal bandga)
 
  for (uint8_t i = 0; i< USARTSPI::nbUARTS; i++){
    YSPI *yyy;
    if (usingUSARTSPI){
      yyy = usartInit(i+usartID0);
    }
    else{  // HW SPI
      yyy = hwInit(); 
    }
    // step 2: AD7689 instantiation
    flashInfo(2);
    delay(1000);
    adcVec[i] = new AD7689(yyy, 8); //nbChannels);
    flash(adcVec[i]);
    (useSerial && Serial.println(String("AD7689 instance :\t") +String(i) + String(" created!")));
    delay(1000);
    // step 3: AD7689 self-test
    flashInfo(3);
    delay(1000);
    doSelfTest(adcVec[i]);
    if(!usingUSARTSPI){
      break;
    }
  }
}

