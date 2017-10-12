#!/usr/bin/python3

# example call
# ./master.py s_init_t s s_payload_t

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

import commsMgr as comms


# global variable shared between threads
fileLockDict = {}

class WriterThread(threading.Thread):
    def __init__(self, name, q, lock, dataDir,syncTimeFunc):
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
        self.getSynchTimeFunc = syncTimeFunc

    def getFormattedRow(self,row):
        row[2]= round(row[2],4)
        return row[1:3]

    def createDataFile(self,outFile):
        headers = ('Timestamp','Value')
        with open(outFile, 'a', newline='') as csvfile:
            writer = csv.writer(csvfile, delimiter=',',
                                quotechar='"', quoting=csv.QUOTE_MINIMAL)
            #print(headers)
            writer.writerow(headers)
            print('created file:',outFile)

    def decodeADCCID(self,coded):
        maskR = 0b1111
        return [((coded>>4)& 0b1111), coded & 0b1111]
            
    def getFile(self,row):
        [adcID,chID] = self.decodeADCCID(row[0])
        filename = self.dataDir + '/{bid}_{adc}_{cid}_{syc}.csv'.format(bid=row[3],adc=adcID,cid=chID,syc=self.getSynchTimeFunc())
        self.dictLock.acquire()
        try:
            self.fileLock = fileLockDict[filename]
        except KeyError:
            self.fileLock = threading.Lock()
            fileLockDict[filename] = self.fileLock
            if not os.path.exists(filename):
                self.createDataFile(filename)
        self.dictLock.release()
        self.fileLock.acquire()
        return filename 

    def releaseFile(self,filename):
        self.fileLock.release()

    def do_work(self,thing):
        filename = self.getFile(thing)
        #print('writing a row',thing)
        with open(filename, 'a', newline='') as csvfile:
            writer = csv.writer(csvfile, delimiter=',',
                                quotechar='"', quoting=csv.QUOTE_MINIMAL)
            writer.writerow(self.getFormattedRow(thing))
        self.releaseFile(filename)


    def run(self):
        """
        Thread run method. 
        """
        try:
            while True:
                item = self.q.get()
                if item is None:
                    break
                self.do_work(item)
                self.q.task_done()
        except Exception as e:
            print('thread exiting...')
        finally:
            self.q.task_done()


###### Master Class CODE ##################

class Master:
    def __init__(self, nbThreads=3):
        self.dataDir = './DATA'
        self.q = queue.Queue()
        self.lock = threading.Lock()
        self.spiCommsMgr = comms.CommsMgr(self.q)
        self.createThreads(nbThreads,self.q,self.lock)
        
    def createThreads(self,num,que,lok):
        self.threads = []
        for i in range(num):
            name='WriterThread-' + str(i)
            t = WriterThread(name,que,lok,'./DATA',self.spiCommsMgr.getSynchTime)
            t.start()
            self.threads.append(t)

    def stopAll(self):
        # block until all tasks are done
        self.q.join()

        # stop workers
        for t in self.threads:
            self.q.put(None)
        for t in self.threads:
            t.join()

    def run(self,typeLis):
        t = time.time()
        try:
            self.spiCommsMgr.loop(typeLis)
        except Exception as e:
            print('main thread exiting...')
            raise
        finally:
            self.stopAll()
            print('Elapsed Time :',time.time()-t,'seconds')

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Usage: $ ./master.py s_init_t, s_payload_t'  )
        print('s_payload_t will be repeated to continue comms indefinitely...')
        print('Note: the AEM board must be running the appropriate software, corresponding to the <type>')
        sys.exit(0)

    master = Master()

    tyLis = list(map(lambda s:eval('comms.'+s),sys.argv[1:]))
    try:
        master.run(tyLis)
    except Exception as e:
        print(e)
        print('exiting...')
                 
        
