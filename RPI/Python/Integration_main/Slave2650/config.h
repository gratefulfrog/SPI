#ifndef CONFIG_H
#define CONFIG_H


/*************************************************************/
/** Types config */
/*************************************************************/
typedef uint32_t timeStamp_t; /*!< typedef for timestamps */
typedef float   ADC_value_t; /*!< typedef for ADC values*/
typedef uint8_t ADCCID_t;    /*!< typedef for ADC AID and CID encoded onto 8 bits */
typedef uint32_t boardID;     /*!< typedef for GUID on 32 unsigned bits */

/*************************************************************/
/** Board config */
/*************************************************************/

#define BOARD_BOARD_ID (11111111)  // 8x 1s

#endif


