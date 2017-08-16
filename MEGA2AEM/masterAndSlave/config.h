#ifndef CONFIG_H
#define CONFIG_H

/*************************************************************/
/** General Values */
/*************************************************************/
//#define DEBUG

/*************************************************************/
/** Q config */
/*************************************************************/
#define Q_Q_LENGTH                   (400) /*!< Max number of elements that the q can contain, note this is used below in config.h */
#define Q_OVERRUN_DELETE_OLDEST      (0)   /*!< if non zero the oldest q elt is deleted in case of overflow, otherwise ne elts are not pushed onto the q when full */

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
typedef struct timeValStruct_t {
  ADCCID_t      aidcid;
  timeStamp_t   t;
  ADC_value_t   v;
} __attribute__((__packed__));
typedef void (*processingUint32FuncPtr)(uint32_t &v);         /*!< typedef for funciton pointer to a function that will process a uint32_t value*/
typedef void (*processingUintTVSFuncPtr)(timeValStruct_t &v); /*!< typedef for funciton pointer to a function that will process a timeValStruct_t */


/*************************************************************/
/** ADC config values */
/*************************************************************/
#define BD0_ADC_NB_CHANNELS (8)  // this may need modification in real life

/*************************************************************/
/** App Config */
/*************************************************************/
#define APP_BIG_BUFF_SIZE              (50)   // bytes
#define APP_SLAVE_PROCESSING_TIME      (0)    // milli seconds
#define APP_PAUSE_BETWEEN_SENDS        (10)   // 10 micro seconds

/*************************************************************/
/** Board config */
/*************************************************************/
#define BOARD_BOARD_0_ID                 (0)  /*!< Board 0 id */
#define BOARD_BOARD_1_ID                 (1)  /*!< Board 1 id */

#define BOARD_BOARD_ID                   (BOARD_BOARD_1_ID)  /*!< this is how the board knows its id in test code, in real code the guid chip will be polled to obtain this number */
#define BOARD_OUTPUT_BURST_LENGTH        (Q_Q_LENGTH)  /*!< How many data structs will be sent back to the Master at every ACQUIRE query*/
#define BOARD_BOARD_0_NB_ADCS            (2)    /*!< nb of adcs connected to board  */
#define BOARD_BOARD_1_NB_ADCS            (1)    /*!< nb of adcs connected to board  */

/**  BOARD_SLAVE_LOOP_ITERATIONS determines how many times the slave will loop betwenn polls of the ADCs.
 *   set the value to (1) for maximum slave speed! */
#define BOARD_SLAVE_LOOP_ITERATIONS      (750) //(1000) // (2150)  //(10150)  // working values // 2150 // 2500 // 5000 

typedef uint32_t boardID; /*!< defines a type for the GUID */

/*************************************************************/
/** ADCMgr config */
/*************************************************************/
/** Special value to indicate that a data strcut can be ignored */
#define ADCMGR_NULL_ADC_ID                (255) 


/*************************************************************/
/** SPI_Anything config */
/*************************************************************/
#define SPI_A_NULL_CHAR             ('#')
#define SPI_A_PAUSE_BETWEEN_SENDS   (20)

/*************************************************************/
/** YSPI config */
/*************************************************************/
// none

/*************************************************************/
/*************************************************************/
#endif

