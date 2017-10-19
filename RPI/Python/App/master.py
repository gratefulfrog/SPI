#!/usr/bin/python3

# example call
# ./master.py 0 1 s_init_t s s_payload_t

""" some results
Wait after clock seems to be a key parameter, too low and we get "errors corrected"
It allows the slave time to react to the SPI interrupt.
The more work the slave does in the interrupt handler, the longer the wait must be.

The following retuls are left for historical purposes and no longer reflect the actual
times of the coded:

2017 10 09 : SPI frequency = 4MHz, 
             wait after clock 30 us
             Q len = 750:
             7000 polls of Q in 7032  sec =  746 items polled/sec
             5251174 lines in data.csv     = 746 lines/sec
             0 errors corrected
             0 state errors
"""

## python system imports
import csv
import threading
import queue
import time
import sys
import os.path

## AEM code imports
import commsMgr as comms


## global package variable shared between threads,
## keys: full filename
## values: threading.lock object used to guarranty exclusive access to the file for writing
fileLockDict = {}

class WriterThread(threading.Thread):
    def __init__(self, name, q, lock, dataDir,syncTimeFunc):
        """
        Consumer class instanciation:
        The Writer thread implements the consumer in the producer/consumer paradigm.
        The number of threads created is not known by the individual threads. They
        simply pop things off the shared thread-safe queue, extract the data, and
        write to the correct data (csv) file, creating it and the data directory if needed.
        The file names for the data files are created from the data elts themselves, in combination
        with the information obtained by callingthe syncTimeFunc provided as argument.
        Data file names are used as keys in the shared fileLockDict, whre the values are semaphores
        ensuring unique access to each file.
        @param self
        @param name a string naming the thread, legacy but left to allow for easier debugging
        @param q the thread-safe q which will be popped to get data
        @param lock the semaphore ensure unique access to the fileLockDict
        @param dataDir the path to the data directory
        @param syncTimeFunc a function that will be called to get the synch time to be used in naming the
        data files.
        """
        threading.Thread.__init__(self)
        ## string name of the thread, used for debugging or information messages
        self.name = name    
        ## work q, source of data to be written to files
        self.q = q
        ## semaphore for exclusive access to the fileLockDict
        self.dictLock = lock
        ## semaphore locking access to the file currently being written
        self.fileLock = None
        ## path to the data file target directory
        self.dataDir = dataDir
        self.dictLock.acquire()
        if not os.path.exists(dataDir):
            os.makedirs(dataDir)
        self.dictLock.release()
        ## function which when called will return the synchronisation time of the boards
        self.getSynchTimeFunc = syncTimeFunc

    def getFormattedRow(self,row):
        """
        Formats a row by rounding the float values to 4 decimals
        @param row the data row,
        @return the row as a list ready to be written to the csv data file.
        """
        row[2]= round(row[2],4)
        return row[1:3]

    def createDataFile(self,outFile):
        """
        Called to create the data csv file and write the header row.
        """
        headers = ('Timestamp','Value')
        with open(outFile, 'a', newline='') as csvfile:
            writer = csv.writer(csvfile, delimiter=',',
                                quotechar='"', quoting=csv.QUOTE_MINIMAL)
            #print(headers)
            writer.writerow(headers)
            print('created file:',outFile)

    def decodeADCCID(self,coded):
        """
        Decodes the ADC and Channel values which were encoded onto a single byte such that
        the top 4 bits are the ADC id, and the lower four are the channel id.
        @param coded a single byte containing the encoded values
        @return a list as decoded into [ADC_ID, Channel_ID]
        """
        maskR = 0b1111
        return [((coded>>4)& 0b1111), coded & 0b1111]
            
    def getFile(self,row):
        """
        the filename is computed, then the dictLock is acquired to access the fileLockDict and
        get a lock for the data file, creating the lock if needed.
        the dictLock is released and the fileLock is aquire before returning
        @param row the data to be used to get the file name
        """
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
        """
        Simply releases the fileLock
        """
        self.fileLock.release()

    def do_work(self,thing):
        """
        This method handles the 'consuming' of the thing popped from the queue.
        After obtaining a lock for the appropriate data file, a csv row is written,
        the lock is released.
        @param the object popped from the queue, in our case a list of length 4:
        ADCCID,Timestamp,value, Board_ID
        """
        filename = self.getFile(thing)
        #print('writing a row',thing)
        with open(filename, 'a', newline='') as csvfile:
            writer = csv.writer(csvfile, delimiter=',',
                                quotechar='"', quoting=csv.QUOTE_MINIMAL)
            writer.writerow(self.getFormattedRow(thing))
        self.releaseFile(filename)


    def run(self):
        """
        Thread run method. pops the queue, sends the popped thing to be consumed.
        if a None value is popped, the method exits properply and the thread ends.
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
    def __init__(self, nbThreads=3,dataDir='./DATA'):
        """
        Constructor for class Master, implements the start and Producer part of
        the Producer/Consumer paradigm for parallel processing. 
        At instanciation, 
        a FIFO, thread-safe queue is created which will be where the producer puts
        his production and where the consumers get it.
        A semaphore (threading.Lock) object is created to ensure unicity of access
        the the fileLockDict used by all the consumer threads.
        An insnce of the spiCommsMgr it created.
        Finally the conusmer threads are created.
        @param self,
        @param nbThreads defaults to the initializer
        @param dataDir default value provided, the directory where the conusmers
        will write the csv files.
        """
        ## Directory for data files
        self.dataDir = dataDir
        ## Synchronized work queue
        self.q = queue.Queue()
        ## Semaphore object to be passed to consumer threads for their use
        self.lock = threading.Lock()
        ## instance of CommsMgr class used to commuicate via SPI with the AEM Slave boards
        self.spiCommsMgr = comms.CommsMgr(self.q)
        self.createThreads(nbThreads)
        
    def createThreads(self,num):
        """
        Creates the number of consumer threads according to the argument.
        @param num the number of threads to create
        """
        self.threads = []
        for i in range(num):
            name='WriterThread-' + str(i)
            t = WriterThread(name,self.q,self.lock,self.dataDir,self.spiCommsMgr.getSynchTime)
            t.start()
            self.threads.append(t)

    def stopAll(self):
        """ 
        Called at the end of a run, it allows all the consumer threads to exit properly
        """
        # block until all tasks are done
        print('Shutting down all threads...')
        self.q.join()

        # stop workers
        for t in self.threads:
            self.q.put(None)
        threadCounter = 0
        for t in self.threads:
            print('Thread', threadCounter,' shut down...')
            threadCounter+=1
            t.join()
        print('All threads shut down, exiting...')

    def run(self,typeLis):
        """
        called to start a data run, after the consumer threads have been started.
        will display the elapsed time on exit.
        To exit, ctrl-c will be handled properly.
        @param tyleLis is a list of the messages to pass on the commsMgr loop. These are values not strings.
        """
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
        print('Usage: $ ./master.py <SS channels defaults to ZERO> s_init_t, s_payload_t'  )
        print('examples;')
        print('Usage: $ ./master.py s_init_t, s_payload_t      # uses default SS Channel 0'  )
        print('Usage: $ ./master.py 0 s_init_t, s_payload_t    # same as previous'  )
        print('Usage: $ ./master.py 1 s_init_t, s_payload_t    # use SS channel 1' )
        print('Usage: $ ./master.py 0 1 s_init_t, s_payload_t  # use SS channels 0 and 1' )
        print('s_payload_t will be repeated to continue comms indefinitely...')
        print('Note: the AEM board must be running the appropriate software, corresponding to the <type>')
        sys.exit(0)

    ## instance of Master class running the entire show!
    master = Master()

    ## contains the Channels extracted from the command line args, properly formatted to be passed to the Master.run method
    channelVec = [0] if len(sys.argv)==3 else list(map(int,sys.argv[1:-2]))
    ## contains the channels and message types, extracted from command line args, properly formatted to be passed to the Master.run method
    tyLis = [channelVec] + list(map(lambda s:eval('comms.'+s),sys.argv[-2:]))

    try:
        master.run(tyLis)
    except Exception as e:
        print(e)
        print('exiting...')
                 
        
