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
#import spi1QueryResponse as comms
import tempspi1QueryResponse as comms

outFile = './DATA/data.csv'

syncTime = ''
fileLockDict = {}

class WriterThread(threading.Thread):
    def __init__(self, name, q, lock, dataDir):
        threading.Thread.__init__(self)
        self.name = name
        self.q = q
        self.dictLock = lock
        self.fileLock = None
        self.dataDir = dataDir
        self.dictLock.acquire()
        if not os.path.exists(dataDir):
            os.makedirs(dataDir)
        self.dictLock.release()

    def getFormattedRow(self,row):
        row[2]= round(row[2],4)
        return row[1:3]

    def createDataFile(self,outFile):
        headers = ('Timestamp','Value')
        with open(outFile, 'a', newline='') as csvfile:
            writer = csv.writer(csvfile, delimiter=',',
                                quotechar='"', quoting=csv.QUOTE_MINIMAL)
            print(headers)
            writer.writerow(headers)
            print('created file:',outFile)

    def decodeADCCID(self,coded):
        maskR = 0b1111
        return [((coded>>4)& 0b1111), coded & 0b1111]
            
    def getFile(self,row):
        [adcID,chID] = self.decodeADCCID(row[0])
        filename = self.dataDir + '/{bid}_{adc}_{cid}_{syc}.csv'.format(bid=row[3],adc=adcID,cid=chID,syc=comms.syncTime)
        self.dictLock.acquire()
        try:
            self.fileLock = fileLockDict[filename]
        except KeyError:
            self.fileLock = threading.Lock()
            fileLockDict[filename] = self.fileLock
            self.createDataFile(filename)
            self.dictLock.release()
        self.fileLock.acquire()
        return filename 

    def releaseFile(self,filename):
        self.fileLock.release()

    def do_work(self,thing):
        filename = self.getFile(thing)
        with open(filename, 'a', newline='') as csvfile:
            writer = csv.writer(csvfile, delimiter=',',
                                quotechar='"', quoting=csv.QUOTE_MINIMAL)
            writer.writerow(self.getFormattedRow(thing))
        self.releaseFile(filename)


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


def createThreads(num,que,lok):
    thrs = []
    for i in range(num):
        name='WriterThread-' + str(i)
        t = WriterThread(name,que,lok,'./DATA')
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

tempBID = 123
tempADC = 0
tempCID = 0
tempTimeStamp = 0
tempVal = 0.0
def tempGetNext():
    global tempBID
    global tempADC
    global tempCID
    global tempTimeStamp
    global tempVal
    res = [(tempADC<<4) | tempCID, tempTimeStamp, tempVal, tempBID]

    tempCID = (tempCID+1)%8
    if tempCID == 0:
        tempADC = (tempADC+1)%2
    tempTimeStamp +=1
    tempVal += 0.1
    print(res)
    return res
        
#def main(numThreads, nbItems):
def main(typeLis,numThreads=3):
    lock = threading.Lock()
    q = queue.Queue()

    threads =  createThreads(numThreads,q,lock)

    t = time.time()
    try:
        #comms.go(typeLis,q)
        q.put(tempGetNext())
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
    
                 
        
