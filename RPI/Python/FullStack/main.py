#!/usr/bin/python3

import csv
import threading
import queue
import time
import sys
import os.path

#import spi1StructWriter as comms
import spi1QueryResponse as comms


outFile = './DATA/data.csv'

class WriterThread(threading.Thread):
    def __init__(self, name, f, q, lock):
        threading.Thread.__init__(self)
        self.name = name
        self.csvfile = f
        self.q = q
        self.lock = lock

    def do_work(self,thing):
        writer = csv.writer(self.csvfile, delimiter=',',
                            quotechar='"', quoting=csv.QUOTE_MINIMAL)
        self.lock.acquire()
        #print(thing)
        outgoing = [self.name] + thing
        writer.writerow(outgoing)
        #print(outgoing)
        self.lock.release()
        

    def run(self):
        """
        Thread run method. Check URLs one by one.
        """
        while True:
            item = self.q.get()
            if item is None:
                break
            self.do_work(item)
            self.q.task_done()


###### MAIN CODE ##################

def getItem(itemCount):
    return [itemCount]*4


def createThreads(num,fil,que,lok):
    thrs = []
    for i in range(num):
        name='WriterThread-' + str(i)
        t = WriterThread(name,fil,que,lok)
        #print(name)
        t.start()
        thrs.append(t)
    return thrs

def enq(q,nbItems):
    for i in range(nbItems):
        item = getItem(i)
        #print('enqueue :', item)
        q.put(item)

def stopAll(q,thr):
    # block until all tasks are done
    q.join()

    # stop workers
    for t in thr:
        q.put(None)
    for t in thr:
        t.join()

def createDataFile():
    headers = ['Thread_Name','ADC_CH_ID','Timestamp','Value', 'Bid']
    with open(outFile, 'w+', newline='') as csvfile:
        writer = csv.writer(csvfile, delimiter=',',
                            quotechar='"', quoting=csv.QUOTE_MINIMAL)
        writer.writerow(headers)        

#def main(numThreads, nbItems):
def main(typeLis,numThreads=3):
    lock = threading.Lock()
    q = queue.Queue()

    if not os.path.exists(outFile):
        createDataFile()

    with open(outFile, 'a', newline='') as csvfile:
        writer = csv.writer(csvfile, delimiter=',',
                            quotechar='"', quoting=csv.QUOTE_MINIMAL)

        threads =  createThreads(numThreads,csvfile,q,lock)

        t = time.time()
        try:
            comms.go(typeLis,q)
        except Exception as e:
            print(e.message,e.args)
        finally:
            stopAll(q,threads)
            print('Elapsed Time :',time.time()-t)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Usage: $ ./spi1Struct.py <type>')
        print('where <type> is one of:   s_init_t, s_bid_t, s_payload_t,  s_wakeup_t'  )
        print('Note: the AEM board must be running the appropriate software, corresponding to the <type>')
        sys.exit(0)

    #main(int(sys.argv[1]),int(sys.argv[2]))
    #main(eval('comms.'+sys.argv[1]))
    tyLis = list(map(lambda s:eval('comms.'+s),sys.argv[1:]))
    main(tyLis)
    
                 
        
