#!/usr/bin/python3

# example call
# ./main.py s_init_t s_bid_t s_wakeup_t s_payload_t

""" some results
SPI frequency = 1MHz
2017 10 08 : Q len = 100: 
             1100 polls in 377 secs = 291 Q items polled/sec 
             114301 lines in data.csv,  303 lines/sec
             1 error corrected!, 
             250 state errors, .002 state errors/line of data
2017 10 08 : Q len = 500:
             400 polls in 311 sec =  643 Q items polled/sec
             201382 lines in data.csv   647 lines/sec
             1 error corrected
             91 state errors, 0.00045 state errors/line of data
2017 10 08 : Q len = 750:
             200 polls in  216 sec =   694 Q items polled/sec
             151746 lines in data.csv   702  lines/sec
             NO errors corrected
             39 state errors,  0.00025 state errors/line of data
SPI frequency = 4MHz
2017 10 08 : Q len = 750:
             600 polls in 633  sec =    710 Q items polled/sec
             450495 lines in data.csv    711 lines/sec
             1 errors corrected
             137 state errors,  0.0003 state errors/line of data
         

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
    
                 
        
