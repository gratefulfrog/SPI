#!/usr/bin/python
import threading
from Queue import *

class MyThread(threading.Thread):
    # Create a subclass of Thread to do the work we need
    def __init__(self, aQueue):
        super(MyThread,self).__init__()
        # be sure to init the parent class
        # the shared Queue is passed as argument on instanciation
        self.q = aQueue
        self.setDaemon(True)
        # setting Daemon ensures a clean exit when program closes,
        # this is a technical detail in Python.

    def run(self):
        # the 'run' method is called by Python when the thread instance 
        # is started.
        print(self.getName() + " started!")
        while (True):
            # forever look at the queue and take a chunk of work
            # if the queue is empty, this blocks allowing the 
            # scheduler to take the thread out of the CPU...
            item = self.q.get()
            # do the work, in this example, print the item
            print ("\n" + self.getName() + " got: " + item + " off the queue!")
            # tell the queue that the work's done
            self.q.task_done()
 
if __name__ == '__main__':
    q = Queue()
    # create a shared Queue to farm out work

    mythread = MyThread(q) 
    # create an instance of my thread subclass with the queue as arg to init
    mythread.start()
    # start up the thread. This call does not block
    
    list = ["a","b","c","d","x"]
    # this is a list of work for the thread to do!
    for e in list:     
        print ("\nmain thread enqueueing: " + e)
        q.put_nowait(e);
        # put each item on the queue, but don't wait for it to be processed.
        # it is also possible to put and block and wait
        
    q.join()
    # this prevents the main thread from exiting while any subthread 
    # is still active. Without it, the shell would kill subthreads when the
    # parent thread exits.
