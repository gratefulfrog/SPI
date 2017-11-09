#ifndef CONFIG_H
#define CONFIG_H

//#include "I2C.h"

/*************************************************************/
/** DEBUG config */
/*************************************************************/
//#define DEBUG

#define SerialUSB Serial

typedef uint8_t BitOrder;


/*************************************************************/
/** Types config */
/*************************************************************/
typedef uint32_t timeStamp_t; /*!< typedef for timestamps */
typedef float    ADC_value_t; /*!< typedef for ADC values*/
typedef uint8_t  ADCCID_t;    /*!< typedef for ADC AID and CID encoded onto 8 bits */
typedef uint32_t boardID;     /*!< typedef for GUID on 32 unsigned bits */

/** This is the stuct used to handle data read from the sensors as well as all SPI responses to the Master
 */
struct u8u32f_struct{
  uint8_t  u8;  //<! this value encodes the ADC id and the Channel id on a byte, high 4 bits are ADC id, low 4 bits are channel id
  uint32_t u32; //<! this is the timestamp value
  float    f;   //<!this is the sensor value
  
  /* operators used in comparisons for this struct type
   */
  boolean operator==(const u8u32f_struct& r) const {
    return (u8 == r.u8) &&
           (u32 == r.u32) &&
           (f == r.f);
  }
  /* operators used in comparisons for this struct type
   */boolean operator!=(const u8u32f_struct& r) const {
    return !(*this == r);
  }
} __attribute__((__packed__));  //<! packing ensures that onlyt the 9 bytes needed by the struct are used in memory


/*************************************************************/
/** Board config */
/*************************************************************/

#define BOARD_USE_BOARD_0                (1)    /*<! select which board will be configured, a true value uses board ZERO otherwise BOARD_1 */


#define BOARD_BOARD_0_ID                 (0)    /*!< ID of board zero  */
#define BOARD_BOARD_0_NB_ADCS            (1)    /*!< nb of adcs connected to board zero */
#define BOARD_BOARD_0_ADC_0_NB_CHANNELS  (8)    /*!< nb of adcs channels for board zero adc zero */

#define BOARD_BOARD_1_ID                 (1)    /*!< ID of board one */
#define BOARD_BOARD_1_NB_ADCS            (5)    /*!< nb of adcs connected to board one */

// the following lines determine the number of channels querried per adc (or assimilated device)
// channels are queried from channel 0,
// there is currently no way to querry a cherry picked subset of the channels,
// if that is needed, then it will require more work
#define BOARD_BOARD_1_ADC_0_NB_CHANNELS  (0)    /*!< max = 8; nb of adcs channels for board one adc zero   AD7689  */
#define BOARD_BOARD_1_ADC_1_NB_CHANNELS  (0)    /*!< max = 8; nb of adcs channels for board one adc one    AD7689  */
#define BOARD_BOARD_1_ADC_2_NB_CHANNELS  (0)    /*!< max = 3; nb of adcs channels for board one adc two    ADXL345 */
#define BOARD_BOARD_1_ADC_3_NB_CHANNELS  (0)    /*!< max = 3; nb of adcs channels for board one adc three  KX122   */
#define BOARD_BOARD_1_ADC_4_NB_CHANNELS  (3)    /*!< max = 3; nb of adcs channels for board one adc four   KX122   */


/*************************************************************/
/** ADCMGR config */
/*************************************************************/
#define ADCMGR_SS_PIN  (10)  //<! pin for ADC slave select

/*************************************************************/
/** SlaveApp config */
/*************************************************************/
  
#define SlaveApp_LED_PIN_1 (35)  //<! AEM board heartbeat led pin 1
#define SlaveApp_LED_PIN_2 (36)  //<! AEM board heartbeat led pin 2
#define SlaveApp_HB_TIME (500)   //<! AEM board led heartbeat flash time in  millisecs

#define SlaveApp_SAY_HEARTBEAT_ONLY (true)  //<! debugging define controls how data the slave prints to serial monitor, leave at TRUE for minimal output
#define SlaveApp_HB_SHOW_COUNT      (300)   //<! the number of data loop iterations before the slave outputs to serial monitor.

#endif


