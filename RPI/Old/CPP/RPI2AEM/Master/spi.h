#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <wiringPiSPI.h>

/** SPI wrapper class provides services that are as close as possible to the Arduino SPI library.
 *  Instance Constructor with channel and speed parameters
 *  tranfer a single byte, as per Arduion SPI_transfer
 *  transfer in place, where the outgoing byte is overwritten by the incoming byte (service not available on Arduino)
 *  getSPDR returns the content of the emulated SPDR register, similar to Arduino direct access SPDR */
class SPI{
 protected:
  uint8_t SPDR;          /*!< will contain the content of last incoming byte */
  const int channel;     /*!< channel for rpi spi */

 public:
  /** constructor
   * @param chan  rpi spi channel
   * @param speed  rpi spi speed */
  SPI(int chan,int speed);
  /** sends the value of inOUT and replaces it with the byte received, SPDR is updated with incoming byte
   * @param &inOut  at call should contain outgoing byte, at return contains incoming byte */
  void transferInPlace(uint8_t &inOut); // not const since SPDR is updated
  /** sends the value of what, is updated with incoming byte, returns incoming byte
   * @param what   outgoing byte, 
   * @return incoming byte */
  uint8_t transfer(const uint8_t what);
  /** returns value of SPDR, ie last incoming byte
   * @return value of last incoming byte */
  uint8_t getSPDR() const;
};

#endif

  
