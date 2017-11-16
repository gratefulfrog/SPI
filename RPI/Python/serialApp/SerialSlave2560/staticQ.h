#ifndef Q_H
#define Q_H

#include <Arduino.h>

/** Constructor of a template Queue class
 * Usage:
 * type T should be an object you want to enqueue, it will be copied to the queue
 * 
 * calls to all methods of this class should be enclosed in noInterrupts()...interrupts() to prevent bad things from
 * happening by re-etnrant code!
 * In case of over-run, the pointer will be used to delete the most recent or last object object, depending 
 * on the value of Q_OVERRUN_DELETE_OLDEST
 */
#define Q_OVERRUN_DELETE_OLDEST (false)
#define Q_Q_LENGTH              (700) // 700 seems to be the max size possible

template <class T> 
class Q{
  protected:
    unsigned int pptr   = 0,  /*!< index of the next spot to put an elt */
                 gptr   = 0,  /*!< index of the next spot to get an elt */
                 qNbObj = 0;  /*!< nb of elts in queue */
    static const unsigned int qLen   = Q_Q_LENGTH; /*!< max nb of elts that the queue can contain */
    static const boolean overrunDeleteOldest = (boolean) Q_OVERRUN_DELETE_OLDEST; /*!< if true when queue is full, delete oldest elt, otherwise do not enqueue new elts  */
    
    T q[qLen];  //<! define the q as being a vector of objects, physically there, not pointers
    T nullObject;  //<! the null object is needed to be returned when q is empty
    
  public:
    /** constructor, sets the nullObject */
    Q(T nullOb) : nullObject(nullOb){
    }
    /** copy a new elt onto the queue
     *  @param elt the elt to be enqueued 
     *  @return boolean TRUE if enqueued */
    boolean push(T elt){
      if (qNbObj == qLen){
        // the q is full, now apply deletion strategy
        if (overrunDeleteOldest){
          // in this case pop the oldest and continue with the enqueing of the current elt.
          //Serial.println("* Q Full! deleting oldest! *");
          pop();
        }
        else{
          // in this case skip the elt that we are attempted to enqueue and return!
          //Serial.println("* Q Full! no push! *");
          return false;
        }
      }
      // this happens if either the Q is not full, or the Q was full and the oldest element has been deleted to make room for one more!
      q[pptr] = elt;
      pptr = (pptr+1) % qLen;
      qNbObj++;
      return true;
    }
    /** pops the oldest elt off the queue
     *  @return the elt popped, or nullObject if queue is empty. the elt is not deleted from heap.
     */
    T pop(){
      T res = nullObject;
      if (qNbObj){
          res = q[gptr];
          gptr = (gptr+1) % qLen;
          qNbObj--;
      }
      return res;
    }
    /** returns the number of objects in the queue
     *  @return number of objects in queue
     */
    unsigned int qNbObjects() const{
      return qNbObj;
    }
    /** returns true if full, false otherwise
     *  @return  true if full, false otherwise
     */
    boolean full() const{
      return (qNbObj == qLen);
    }
    /** return nb elts in queue
     *  @return nb elts in queue
     */
    int qLength() const{
      return qLen;
    }
};

#endif

