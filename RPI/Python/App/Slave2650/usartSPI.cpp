
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

// test to see if we have an ATmega328p or an ATmega2560 MCU
#if ( SIGNATURE_1 == 0x98 && SIGNATURE_2 == 0x01)

///// ATMEGA 2560 VALUES ////////////

const uint8_t USARTSPI::nbUARTS =  4;

// usart reg struct: {&udr,&ucsrA,&ucsrB,&ucsrC,&ubrr}
const USARTSPI::usartRegisterStruct USARTSPI::usartRegVec[nbUARTS] = {{UDR0,UCSR0A,UCSR0B,UCSR0C,UBRR0},  // usart 0
                                                                      {UDR1,UCSR1A,UCSR1B,UCSR1C,UBRR1},  // usart 1
                                                                      {UDR2,UCSR2A,UCSR2B,UCSR2C,UBRR2},  // usart 2
                                                                      {UDR3,UCSR3A,UCSR3B,UCSR3C,UBRR3},  // usart 3
                                                                      }; 

// pin reg struct: {&io,&set}
const USARTSPI::pinRegisterStruct  USARTSPI::pinRegVec[nbUARTS]  = {{DDRE, PORTE},  // usart 0: io: 'D', Set: 'PORTD'
                                                                    {DDRD, PORTD},  // usart 1
                                                                    {DDRH, PORTH},  // usart 2
                                                                    {DDRJ, PORTJ},  // usart 3
                                                                    }; 
struct uartSpecificBitValueStruct {
    const uint8_t   udre,    // UART Data Register Empty bit
                    rxc,     // Receive Complete Bit,
                    txc,     // Transmit Complete Bit
                    ddCLK,   // Data Direction bit for the specific  CLK pin
                    ddSS,    // Data Direction bit for the specific SS pin
                    umsel0,  // USART Mode Select bit 0 for the specific USART channel 
                    umsel1,  // USART Mode Select bit 1 for the specific USART channel 
                    txen,    // Transmitter Enable BIT
                    rxen,    // Receiver Enable BIT
                    portSS;  // Digital Pin Value Set bit for the specific SS pin
  };
                                                                             //  udre  rxc  txc   ddCLK ddSS  umsel0   umsel1   txen   rxen   portSS 
const USARTSPI::uartSpecificBitValueStruct USARTSPI::specificBitVec[nbUARTS] = {{UDRE0,RXC0,TXC0, DDE2, DDE3, UMSEL00, UMSEL01, TXEN0, RXEN0, PORTE3},
                                                                                {UDRE1,RXC1,TXC1, DDD5, DDD4, UMSEL10, UMSEL11, TXEN1, RXEN1, PORTD4},
                                                                                {UDRE2,RXC2,TXC2, DDH2, DDH3, UMSEL20, UMSEL21, TXEN2, RXEN2, PORTH3},
                                                                                {UDRE3,RXC3,TXC3, DDJ2, DDJ3, UMSEL30, UMSEL31, TXEN3, RXEN3, PORTJ3}
                                                                                };
#else

///// ATMEGA 328P VALUES ////////////

static const uint8_t USARTSPI::nbUARTS =  1;

// usart reg struct: {&udr,&ucsrA,&ucsrB,&ucsrC,&ubrr}
const USARTSPI::usartRegisterStruct USARTSPI::usartRegVec[nbUARTS] = {{UDR0,UCSR0A,UCSR0B,UCSR0C,UBRR0}}; // usart 0

// pin reg struct: {&io,&set}
const USARTSPI::pinRegisterStruct  USARTSPI::pinRegVec[nbUARTS]  = {{DDRD,PORTD}};   // usart 0: io: 'D', Set: 'PORTD'
                                                                    
/* struct uartSpecificBitValueStruct {
    const uint8_t   udre,    // UART Data Register Empty bit
                    rxc,     // Receive Complete Bit,
                    txc,     // Transmit Complete Bit
                    ddCLK,   // Data Direction bit for the specific  CLK pin
                    ddSS,    // Data Direction bit for the specific SS pin
                    umsel0,  // USART Mode Select bit 0 for the specific USART channel 
                    umsel1,  // USART Mode Select bit 1 for the specific USART channel 
                    txen,    // Transmitter Enable BIT
                    rxen,    // Receiver Enable BIT
                    portSS;  // Digital Pin Value Set bit for the specific SS pin
  };
 */
const USARTSPI::uartSpecificBitValueStruct USARTSPI::specificBitVec[nbUARTS] = {{UDRE0,RXC0,TXC0, DDD4, DDD5, UMSEL00, UMSEL01, TXEN0,RXEN0, PORTD5}};

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// USARTSPI Class Methods start here
// NOTE: Please do not remove comments so we can understand what is going on!!!
///////////////////////////////////////////////////////////////////////////////////////////////////////////

USARTSPI::USARTSPI(uint8_t usartID) : YSPI(), uID(usartID) {
  // pinMode (MSPIM_SS, OUTPUT);   // MSPIM_SS is pin 5 
  // DDRD |= _BV(DDD5);
  // pinRegVec[uID].io |= _BV(DDD5);
  pinRegVec[uID].io |= _BV(specificBitVec[uID].ddSS);
  
  // must be zero before enabling the transmitter
  // UBRR0 = 0;
  usartRegVec[uID].ubrr = 0;

  // UCSR0A = _BV (TXC0);  // any old transmit now complete
  usartRegVec[uID].ucsrA = _BV (specificBitVec[uID].txc);
  
  // pinMode (MSPIM_SCK, OUTPUT);   // set XCK pin as output to enable master mode MSPIM_SCK is pin 4
  // CKn_DDR |= (1<<XCKn);
  // DDRD |= _BV(DDD4);
  // pinRegVec[uID].io |= _BV(DDD4);
  pinRegVec[uID].io |= _BV(specificBitVec[uID].ddCLK);
  
  // UCSR0C = _BV (UMSEL00) | _BV (UMSEL01);  // Master SPI mode
  // usartRegVec[uID].ucsrC = _BV (UMSEL00) | _BV (UMSEL01);  // Master SPI mode
  usartRegVec[uID].ucsrC = _BV (specificBitVec[uID].umsel0) | _BV (specificBitVec[uID].umsel1);  // Master SPI mode
  
  // UCSR0B = _BV (TXEN0) | _BV (RXEN0);  // transmit enable and receive enable
  usartRegVec[uID].ucsrB = _BV (specificBitVec[uID].txen) | _BV (specificBitVec[uID].rxen);  // transmit enable and receive enable
  
  // must be done last, see page 206
  // UBRR0 = 0; // full speed //3 => 2 Mhz clock rate
  usartRegVec[uID].ubrr = 0;        
}

void USARTSPI::setSS(uint8_t highLow) const{
  //digitalWrite(MSPIM_SS , highLow);
  // MSPIM_SS is pin 5
  if (highLow){
    // set high, i.e. set bit
    // PORTD |= _BV(PORTD5);
    // pinRegVec[uID].set |= _BV(PORTD5);
    pinRegVec[uID].set |= _BV(specificBitVec[uID].portSS);
  }
  else {
    // set low, i.e clear bit
    // PORTD &= ~_BV(PORTD5);
    // pinRegVec[uID].set &= ~_BV(PORTD5);
    pinRegVec[uID].set &= ~_BV(specificBitVec[uID].portSS);
  }
}
void USARTSPI::beginTransaction() const{
  setSS(LOW);
}

// sends/receives one byte
uint8_t USARTSPI::transfer (uint8_t data) const{
  // wait for transmitter ready
  // while (!(UCSR0A & _BV (UDRE0)));
  while (!(usartRegVec[uID].ucsrA & _BV (specificBitVec[uID].udre)));
  
  // send byte
  // UDR0 = data;
  usartRegVec[uID].udr = data;
  
  // wait for receiver ready
  // while (!(UCSR0A & _BV (RXC0)));
  while (!(usartRegVec[uID].ucsrA & _BV (specificBitVec[uID].rxc)));
  
  // receive byte, return it
  // return UDR0;
  return usartRegVec[uID].udr;
}  

void USARTSPI::endTransaction() const{
  // wait for all transmissions to finish
  // while (!(UCSR0A & _BV (TXC0)));
  while (!(usartRegVec[uID].ucsrA & _BV (specificBitVec[uID].txc)));

  // disable slave select
  // digitalWrite (MSPIM_SS, HIGH); 
  // PORTD |= _BV(PORTD5);
  setSS(HIGH);
}


