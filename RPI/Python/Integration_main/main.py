#!/usr/bin/python3

# example call
# ./main.py s_init_t s_bid_t s_wakeup_t s_payload_t

""" some results

wait after clock seems to be a key parameter, too low and we get "errors corrected"

2017 10 09 : SPI frequency = 4MHz, 
             wait after clock 25 us
             Q len = 750:
             200 polls of Qin 192  sec =     781 items polled/sec
             155915 lines in data.csv    812 lines/sec
             1 or 2 errors corrected
             0 state errors
2017 10 09 : SPI frequency = 4MHz, 
             wait after clock 30 us
             Q len = 750:
             7000 polls of Q in 7032  sec =  746 items polled/sec
             5251174 lines in data.csv     = 746 lines/sec
             0 errors corrected
             0 state errors


"""
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

    def getFormattedRow(self,row):
        row[2]= round(row[2],4)
        return row

    def do_work(self,thing):
        writer = csv.writer(self.csvfile, delimiter=',',
                            quotechar='"', quoting=csv.QUOTE_MINIMAL)
        self.lock.acquire()
        #print(thing)
        #outgoing = [self.name] + thing
        #writer.writerow(self.getFormattedRow(outgoing))
        writer.writerow(self.getFormattedRow(thing))
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
    #headers = ['Thread_Name','ADC_CH_ID','Timestamp','Value', 'Bid']
    headers = ['ADC_CH_ID','Timestamp','Value', 'Bid']
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
            print(e) #.message,e.args)
        finally:
            stopAll(q,threads)
            print('Elapsed Time :',time.time()-t)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Usage: $ ./spi1Struct.py <type>')
        print('where <type> is one of:   s_init_t, s_bid_t, s_payload_t,  s_wakeup_t'  )
        print('e.g.:  ./main.py  s_init_t, s_bid_t, s_wakeup_t, s_payload_t')
        print('pairs [s_wakeup_t, s_payload_t] will be added to continue comms indefinitely...')
        print('Note: the AEM board must be running the appropriate software, corresponding to the <type>')
        sys.exit(0)

    #main(int(sys.argv[1]),int(sys.argv[2]))
    #main(eval('comms.'+sys.argv[1]))
    tyLis = list(map(lambda s:eval('comms.'+s),sys.argv[1:]))
    main(tyLis)
    
                 
        
