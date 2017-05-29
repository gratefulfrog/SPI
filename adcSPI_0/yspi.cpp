
/*
Example of USART in SPI mode on the Atmega328.

Author:   Nick Gammon
Date:     12th April 2012
Version:   1.0

Licence: Released for public use.

Pins: D0 MISO (Rx)
      D1 MOSI (Tx)
      D4 SCK  (clock)
      D5 SS   (slave select)  <-- this can be changed

 Registers of interest:

 UDR0 - data register

 UCSR0A – USART Control and Status Register A
     Receive Complete, Transmit Complete, USART Data Register Empty

 UCSR0B – USART Control and Status Register B
     RX Complete Interrupt Enable, TX Complete Interrupt Enable, Data Register Empty Interrupt Enable ,
     Receiver Enable, Transmitter Enable

 UCSR0C – USART Control and Status Register C
     Mode Select (async, sync, SPI), Data Order, Clock Phase, Clock Polarity

 UBRR0L and UBRR0H - Baud Rate Registers - together are UBRR0 (16 bit)

*/

#include "yspi.h"

// sends/receives one byte
byte YMSPI::MSPIMTransfer (byte data){
  // wait for transmitter ready
  while (!(UCSR0A & _BV (UDRE0)));
    
  // send byte
  UDR0 = data;
  
  // wait for receiver ready
  while (!(UCSR0A & _BV (RXC0)));
  
  // receive byte, return it
  return UDR0;
}  


YMSPI::YMSPI(uint8_t usartID) {
  switch (usartID){
  case 0:
    break;
  case 1:
    //pinMode (MSPIM_SS, OUTPUT);   // MSPIM_SS is pin 5 
    DDRD |= _BV(DDD5);
    
    // must be zero before enabling the transmitter
    UBRR0 = 0;
  
    UCSR0A = _BV (TXC0);  // any old transmit now complete
    
    //pinMode (MSPIM_SCK, OUTPUT);   // set XCK pin as output to enable master mode MSPIM_SCK is pin 4
    DDRD |= _BV(DDD4);
    
    UCSR0C = _BV (UMSEL00) | _BV (UMSEL01);  // Master SPI mode
    UCSR0B = _BV (TXEN0) | _BV (RXEN0);  // transmit enable and receive enable

    // must be done last, see page 206
    UBRR0 = 0; // full speed //3 => 2 Mhz clock rate
    break;
  case 2:
    break;
  case 3:
    break;
  }
}

void YMSPI::setSS(int highLow){
  //digitalWrite(MSPIM_SS , highLow);
  // MSPIM_SS is pin 5
  if (highLow){
    // set high, i.e. set bit
    PORTD |= _BV(PORTD5);
  }
  else {
    // set low, i.e clear bit
    PORTD &= ~_BV(PORTD5);
  }
}
void YMSPI::endTransaction(){
  // wait for all transmissions to finish
  while (!(UCSR0A & _BV (TXC0)));

  // disable slave select
  //digitalWrite (MSPIM_SS, HIGH); 
  PORTD |= _BV(PORTD5);
}

