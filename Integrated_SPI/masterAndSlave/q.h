#ifndef Q_H
#define Q_H

#include <Arduino.h>

#define Q_LENGTH (400) // (580)
#define OVERRUN_DELETE_OLDEST (0)
/** Constructor of a template Queue class
 * Usage:
 * type T should be a HEAP allocated object you want to enqueue, but only a POINTER to it will be enqueued.
 * 
 * calls to all methods of this class should be enclosed in noInterrupts()...interrupts() to prevent bad things from
 * happening by re-etnrant code!
 * In case of over-run, the pointer will be used to delete the most recent or last object object, depending 
 * on the value of OVERRUN_DELETE_OLDEST
 */
template <class T> 
class Q{
  protected:
    volatile unsigned int pptr   = 0,  /**< volatile (for use via interrupt) index of the next spot to put an elt */
                          gptr   = 0,  /**< volatile (for use via interrupt) index of the next spot to get an elt */
                          qNbObj = 0;  /**< volatile (for use via interrupt) nb of elts in queue */
    static const unsigned int qLen   = Q_LENGTH; /**< max nb of elts that the queue can contain */
    static const boolean overrunDeleteOldest = (boolean) OVERRUN_DELETE_OLDEST; /**< if true when queue is full, delete oldest elt, otherwise do not enqueue new elts  */
    T *q[qLen];
    
  public:
    /** constructor, sets all pointers to NULL */
    Q(){
      // this init is not really necessary, but I like things to be clean in case of a future need to debug.
      for (unsigned int i = 0;  i < qLen;i++){
        q[i] = NULL;
      }
    }
    /** push a pointer to a new elt onto the queue
     *  @param *elt pointer to the let to be enqueued */
    void push(T *elt){
      if (qNbObj == qLen){
        // the q is full, now apply deletion strategy
        if (overrunDeleteOldest){
          // in this case delete the oldest and continue with the enqueing of the current elt.
          Serial.println("************************** Q Full! deleting oldest! *******************************");
          delete pop();
          qNbObj--;
        }
        else{
          // in this case delete the elt that we are attempted to enqueue and return!
          Serial.println("************************** Q Full! no push! *******************************");
          delete elt;
          return;
        }
      }
      // this happens if either the Q is not full, or the Q was full and the oldest element has been deleted to make room for one more!
      q[pptr] = elt;
      pptr = (pptr+1) % qLen;
      qNbObj++;
    }
    /** pops the oldest elt off the queue
     *  @return a pointer to the elt popped, or NULL if queue is empty. the elt is not deleted from heap.
     */
    T* pop(){
      T* res = NULL;
      if (qNbObj){
          res = q[gptr];
          gptr = (gptr+1) % qLen;
          qNbObj--;
      }
      return res;
    }
    unsigned int qNbObjects() const{
      return qNbObj;
    }
};

#endif

