#ifndef CONFIG_H
#define CONFIG_H

/*************************************************************/
/** Types config */
/*************************************************************/
typedef uint32_t timeStamp_t; /*!< typedef for timestamps */
typedef float    ADC_value_t; /*!< typedef for ADC values*/
typedef uint8_t  ADCCID_t;    /*!< typedef for ADC AID and CID encoded onto 8 bits */
typedef uint32_t boardID;     /*!< typedef for GUID on 32 unsigned bits */

/*************************************************************/
/** Board config */
/*************************************************************/

#define BOARD_USE_BOARD_0                (1)    /*<! slecect which board will be configured, a true value uses board ZERO otherwise BOARD_1 */


#define BOARD_BOARD_0_ID                 (0)    /*!< nb of adcs connected to board  */
#define BOARD_BOARD_0_NB_ADCS            (1)    /*!< nb of adcs connected to board zero */
#define BOARD_BOARD_0_ADC_0_NB_CHANNELS  (8)    /*!< nb of adcs channels for board zero adc zero */

#define BOARD_BOARD_1_ID                 (1)    /*!< nb of adcs connected to board  */
#define BOARD_BOARD_1_NB_ADCS            (2)    /*!< nb of adcs connected to board one */
#define BOARD_BOARD_1_ADC_0_NB_CHANNELS  (8)    /*!< nb of adcs channels for board one adc zero */
#define BOARD_BOARD_1_ADC_1_NB_CHANNELS  (8)    /*!< nb of adcs channels for board one adc one */



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


