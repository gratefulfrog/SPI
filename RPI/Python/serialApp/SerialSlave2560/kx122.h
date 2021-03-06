#ifndef YKX122_H
#define YKX122_H

#include <Arduino.h>

#include "yspi.h"
#include "yADC.h"
#include "I2C.h"

#define  KX122_XOUT_L   (0x06)
#define  KX122_XOUT_H   (0x07)
#define  KX122_YOUT_L   (0x08)
#define  KX122_YOUT_H   (0x09)
#define  KX122_ZOUT_L   (0x0A)
#define  KX122_ZOUT_H   (0x0B)

#define KX122_WHOAMI   (0x0F)
#define KX122_I_AM_WHO (0x1B)

#define KX122_GET_ACC  (0x06)

#define KX122_2G       (0x00)
#define KX122_4G       (0x08)
#define KX122_8G       (0x10)
#define KX122_LOW_RES  (0x00)
#define KX122_HI_RES   (0x40)
#define KX122_OPERATE  (0x80)

#define KX122_DR_12_5  (0x00)
#define KX122_DR_25    (0x01)
#define KX122_DR_50    (0x02)
#define KX122_DR_100   (0x03)
#define KX122_DR_200   (0x04)
#define KX122_DR_400   (0x05)
#define KX122_DR_800   (0x06)
#define KX122_DR_1600  (0x07)
#define KX122_DR_0_781 (0x08)
#define KX122_DR_1_563 (0x09)
#define KX122_DR_3_125 (0x0A)
#define KX122_DR_6_25  (0x0B)
#define KX122_DR_3200  (0x0C)
#define KX122_DR_6400  (0x0D)
#define KX122_DR_12800 (0x0E)
#define KX122_DR_25600 (0x0F)

#define KX122_IIR_BYPASS (0x80)
#define KX122_LPRO       (0x40)
#define KX122_OSA3       (0x08) 
#define KX122_OSA2       (0x04) 
#define KX122_OSA1       (0x02) 
#define KX122_OSA0       (0x01)

#define KX122_CNTL1      (0x18) // main features control registger, need to set in 2 phases,
                                // first without operational mode, 
                                // then set operational
#define KX122_ODCNTL     (0x1B) // output data rate control register


/** YKH122
 * Represents the  KX122, acts like an ADC with 3 channels
 * does not use  YSPI functionality since it is an I2C device
 */
class YKX122 : public YADC {
 protected:
  static const uint8_t regVec[][2]; /*! The vector of register, value pairs used in initial configuration of the device */
  const uint8_t addr;               /*! The hardware adresss for this instance for all the KX122s in the sestem */
  void  configKX122() const;        /*! call this at instanciation to set up the device */
  
 public:
  static const uint8_t addrVec[];  /*! The vector of hardware adresses for all the KX122s in the sestem */
  YKX122(uint8_t address, const YSPI *const y = NULL);  /*! constructor does not need the YPSI pointer */
  float acquireChannel(uint8_t channel) ;   /*! as per parent class but 0=x, 1=y, 2=z */
  bool selftest(void) ;                     /*! runs a simple selftest */
  uint8_t getAddr() const;                  /*! getter method for the device's hardware address */
};
#endif


