#include <Arduino.h>

#define NB_ITER (10000)
#define NB_BYTES (6300)

//#define SerialUSB Serial

void setup() {
  uint8_t vec[NB_BYTES];

  SerialUSB.begin(2000000);
  while(!SerialUSB);

  for (int i = 0;i<NB_BYTES;i++){
    vec[i] = 'a';
  }
  
  int now = millis();
  for (int i = 0;i<NB_ITER;i++){
    SerialUSB.write(vec,NB_BYTES);
  }
  int taken = millis()-now;
  
  //SerialUSB.println(SerialUSB.baud());
  
  SerialUSB.end();
  delay(2000);

  float bps = 8*NB_ITER*NB_BYTES*1000.0/taken;
  
  SerialUSB.println();
  SerialUSB.println();
  SerialUSB.println();
  SerialUSB.println();
  SerialUSB.println();
  SerialUSB.println(String("Bps: ") + String(bps));
  // that's what we got: 778 470.44 bps, not a lot!  (he BAUD rate makes no difference!
  // it goes as fast as it can??, nor does the NB_BYTES, unless very small, less than 64 !
}

void loop() {}
