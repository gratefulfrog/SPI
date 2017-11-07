  #ifndef APP_H
#define APP_H

#include <Arduino.h>

#include "config.h"
#include "staticQ.h"
#include "timeStamper.h"
#include "board.h"

/** SlaveApp handles all SPI interactions with master and polls the board for ADC data
 */
class SlaveApp{
  protected:
    /////////////////////////////////////////////
    // definition of the States used in state-machine logic
    /////////////////////////////////////////////
    enum class State  {unstarted,
                       started,
                       initialized,
                       working,
                       readyToSend,
                       sendingStructs};
    /////////////////////////////////////////////
    // Variables only used within main code
    /////////////////////////////////////////////
    u8u32f_struct nullStruct = {255,0,0.0};  //<! null struct used to inform master that there is no more to send

    State previousState = State::unstarted;  //<! used to check if the new state is correct after a transition
    Q<u8u32f_struct> *q;                     //<! poointer to the local q where sensor data is stored before sending to master
    Board *board;                            //<! pointer to board instance which handles the adc interactions

    uint8_t nbActiveADCS,                 //<! of all the adcs on the board this is the number that are going to be used, is <- board_nbADCS
            *adcIndexVec;                 /*<! this vector contains the indexes of all the active adcs, eg. if thereare 2 actve, 1 and 3, this would
                                               look like {1,3} 
                                           */
    State currentState = State::started;  //<! current state of the SlaveApp state machine
    State nextState = State::unstarted;   //<! next state to be set in SlaveApp::loop when SPI ISR returns

    /////////////////////////////////////////////
    // protected Main code methods
    /////////////////////////////////////////////
    /** sync start up of serial comms
     */
    void handShake();
    /** setup calls for heartbeat led pins
     */
    void setupHBLed();
    /** update the heartbeat flashing
     */
    void stepHB() const;
    /** Display the heartbeat infor and current state if full data display is active
     */
    void sayState() const;
    /** compare current state with previous state and dispaly error message if incorrect
     */
    void checkCurrentState() const;
    /** Called to initialize the nbActiveADCS and the adcIndexVector so that in the querying, there will be no wasted cycles  
     *  @param board_nbADCS: total number of adcs available on the board
     *  @param *bordNbChannelVec, a vector of all the adc channels available
     */
    void setupADCVectors(uint8_t board_nbADCS, const uint8_t *bordNbChannelVec);
    /** Called to creat the instance of the timeStamper class and set t0 appropriately
     */
    void createTimeStamper();
    /** as long as there is room on the queue, poll the adc/channels and push the data to the queue.
     *  If there is no room left on the queue, return State::readyToSend to inform the main loop
     *  @return State::working if there is still room on the queue for more data, or State::readyToSend otherwise
     */
    State doWork();
    /** as long as there are elts in the queue, pop and send via serial     
     *  If there are noneleft on the queue, return State::working to inform the main loop, otherwise state remains same
     *  @return State::working if there arenno more on the queue for more data, or State::readyToSend otherwise
     */
    State doSingleSend();
    
  public:
    /** constructor: 
     *  activates Serial
     *  creates all member variables
     *  provides output to Serial to inform 
     *  sets up SPI slave mode
     */
    SlaveApp();
    /** called by the main loop, manages the slave state machine by:
     *  updating state as needed
     *  then perform any actions required by the current state.
     */
    void loop();
};

#endif
