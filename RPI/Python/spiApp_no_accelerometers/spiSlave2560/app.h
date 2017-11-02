  #ifndef APP_H
#define APP_H

#include <Arduino.h>

#include "config.h"
#include "staticQ.h"
#include "timeStamper.h"
#include "board.h"


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
    State previousState = State::unstarted;  //<! used to check if the new state is correct after a transition
    Q<u8u32f_struct> *q;                     //<! poointer to the local q where sensor data is stored before sending to master
    Board *board;                            //<! pointer to board instance which handles the adc interactions
    
    /////////////////////////////////////////////
    // Variables only used within ISR code
    /////////////////////////////////////////////
    const uint32_t init32  = 128,  //<! initial value used as placeholder and in debugging
                   bid32   = 144;  //<! initial value used as placeholder and in debugging
    const uint8_t init8      = 0b10000000,   // DEC 128  //<! byte defining the s_init_t poll from master, after re-assembly
                  payload8   = 0b10110000;   // DEC 176  //<! byte defining the s_payload_t poll from master, after re-assembly
    
    u8u32f_struct *outgoing;  //<! address of the current outgoing struct, for SPI to master
    uint8_t *bytePtr;         //<! address of the current outgoing byte for SPI to master
    u8u32f_struct initResponseStruct =  {init8, init32,0},   //<! struct sent in response to the s_ini_t, but init32 will be replaced by GUID
                  payload = {255,255,255.0},                 //<! struct to be filled and sent to master during payload query/repsnse
                  nullStruct = {255,0,0.0};                  //<! null struct used to inform master that there is no more to send
                  
    /////////////////////////////////////////////
    // variables shared between ISR and main code
    /////////////////////////////////////////////
    volatile State currentState = State::started;  //<! current state of the SlaveApp state machine
    volatile State nextState = State::unstarted;   //<! next state to be set in SlaveApp::loop when SPI ISR returns
    volatile boolean flag = false;                 //<! flag used in loop to trigger a state change
    /////////////////////////////////////////////

    /////////////////////////////////////////////
    // protected ISR methods 
    /////////////////////////////////////////////
    /** tests arg to see if is coherent with a slave outgoing msg. Used to avoid errors in SPI
     *  @param msg a byt that will have at least one 1 in the top 4 bits if it is a Master msg
     *  @return true if slave message, false if not
     */
    boolean isSlaveMsg(byte msg) const;   
    /** Method handles all the logic of interpretting incoming bytes from the master and constructing the appropriate responses
     *  for the next exchanges.
     *  The incomoing byte can be a s_init_t, s_payload_t or 1,2, or 3 each left shifted 4 bits to make them master messages.
     *  Depending on the byte received, the slave will do the appropriate action:
     *  if it is a s_init_t or an s_payload_t, a new outgoing struct is filled, and the bytepointer is set to the top 1/2 of the first byte.
     *  if it is a 1, then the the byte ptr is set to the lower 1/2 of the current byte,
     *  if it is a 2, then the byte ptr is set to the 1st 1/2 of the next byte
     *  if is something else, then processing is terminated and the next state is tested to see if the state change flag should be set. the byte ptr is reset.
     *  @param incoming the byte received from the master
     *  @return the byte to send back, knowing that it will only be read in the next exchange!
     */
    byte response(byte incoming);
    /** Depending on the msg type received and the current state, a pointer to an appropriate response stuct is provided,
     *  In side-effect, the nextState is computed and assigned
     * @param type is the incoming byte from the master either init8 or payload8
     * @return pointer to the struct that should be sent back in response
     */
    u8u32f_struct* getOutgoing(uint8_t type) ;
    /** will pop the local queue for the next outgoing value struct. If the q returns an elt, it is copied to the payload struct, if not nullStruct is used meaning that there is nothing left to send
     *  @return True if there is more to send, false if not.
     */
    boolean incOutgoing();
    
    /////////////////////////////////////////////
    // protected Main code methods
    /////////////////////////////////////////////
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
    /** Called to creat the instance of the timeStamper class and set t0 appropriately
     */
    void createTimeStamper();
    /** as long as there is room on the queue, poll the adc/channels and push the data to the queue.
     *  If there is no room left on the queue, return State::readyToSend to inform the main loop
     *  @return State::working if there is still room on the queue for more data, or State::readyToSend otherwise
     */
    State doWork();
    
  public:
    /** constructor: 
     *  activates Serial
     *  creates all member variables
     *  provides output to Serial to inform 
     *  sets up SPI slave mode
     */
    SlaveApp();
    /** called by the main loop, manages the slave state machine by:
     *  1st checking the 'flag' that may have been set in an SPI interrupt, and updating state as neede,
     *  then, checking curreent state against prevous, inform if error, and inform of change in data display is such
     *  then perform any actions required by the current state.
     */
    void loop();
    /** the SPI transmission complete interrupt service routine,
     *  looks at the SPDR and if it is not an outgoing slave mesage, processes it appropriately
     */
    virtual void SPI_ISR ();    
};

#endif
