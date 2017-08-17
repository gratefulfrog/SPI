#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <time.h>


/*************************************************************/
/** General Values */
/*************************************************************/
//#define DEBUG


/*************************************************************/
/** Utilities config */
/*************************************************************/
typedef uint32_t timeStamp_t; /*!< typedef for timestamps */
typedef float   ADC_value_t; /*!< typedef for ADC values*/
typedef uint8_t ADCCID_t;    /*!< typedef for ADC AID and CID encoded onto 8 bits */

/** timeValStruct_t definition. This struct holds:
 *  aidcid: an encoded value with ADC id and Channel Id encoded as AAAACCCC on 8 bits, the value 255 special and indcates a NULL struct to be ignored
 *  
 *  t: is the timestamp value of the data
 *  
 *  v: is the reading from the ADC channel */
/*typedef*/
struct timeValStruct_t {
  ADCCID_t      aidcid;
  timeStamp_t   t;  
  ADC_value_t   v;
}__attribute__((__packed__));

typedef void (*processingUint32FuncPtr)(uint32_t &v);         /*!< typedef for funciton pointer to a function that will process a uint32_t value*/
typedef void (*processingUintTVSFuncPtr)(timeValStruct_t &v); /*!< typedef for funciton pointer to a function that will process a timeValStruct_t */


/*************************************************************/
/** App Config */ 
/*************************************************************/
#define APP_BIG_BUFF_SIZE              (50)     // bytes
#define APP_SLAVE_PROCESSING_TIME      (0)      // seconds
#define APP_PAUSE_BETWEEN_SENDS        (20)     // 10 micro seconds
#define APP_SPI_CHANNEL                (0)      // channel for SPI
#define APP_SPI_SPEED                  (200000) // speed for SPI
#define APP_NULL_AIDCID                (255)    // meaning that the tvs should be ignored

/*************************************************************/
/** FileMgr config */
/*************************************************************/
typedef uint32_t boardID; /*!< defines a type for the GUID */

#define BOARD_BOARD_0_ID                 (0)  /*!< Board 0 id */
#define BOARD_BOARD_1_ID                 (1)  /*!< Board 1 id */

#define BOARD_BOARD_ID                   (BOARD_BOARD_1_ID)  /*!< this is how the board knows its id in test code, in real code the guid chip will be polled to obtain this number */
#define BOARD_BOARD_0_NB_ADCS            (2)    /*!< nb of adcs connected to board  */
#define BOARD_BOARD_1_NB_ADCS            (1)    /*!< nb of adcs connected to board  */

#define BOARD_BOARD_0_ADC_0_NB_CHANNELS  (8)    /*!< nb of channels for the board_adc  */
#define BOARD_BOARD_0_ADC_1_NB_CHANNELS  (8)    /*!< nb of channels for the board_adc  */
#define BOARD_BOARD_1_ADC_0_NB_CHANNELS  (8)    /*!< nb of channels for the board_adc  */

#define HEADER_0                         ("ADC")
#define HEADER_1                         ("Channel")
#define HEADER_2                         ("TimeStamp")
#define HEADER_3                         ("Value")

/*************************************************************/
/** SPI_Anything config */
/*************************************************************/
#define SPI_A_NULL_CHAR             ('#')
#define SPI_A_PAUSE_BETWEEN_SENDS   (20)

/*************************************************************/
/*************************************************************/

#endif

