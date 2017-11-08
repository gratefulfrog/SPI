#include <Arduino.h>

void setup() {
  uint8_t vec[1000];

  SerialUSB.begin(1210000);
  while(!SerialUSB);

  int nb = 1000;
  
  long now = micros();
  for (int i = 0;i<nb;i++){
    SerialUSB.write(vec,1000);
  }
  long taken = micros()-now;
  
  SerialUSB.println(SerialUSB.baud());
  SerialUSB.end();
  delay(2000);
  
  SerialUSB.println();
  SerialUSB.println();
  SerialUSB.println();
  SerialUSB.println();
  SerialUSB.println();
  SerialUSB.println(String("Time taken for ") + String(nb) + String(" bytes in us: ") + String(taken));
 //

}

void loop() {
  // put your main code here, to run repeatedly:

}
