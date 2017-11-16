#!/usr/bin/python3

# example call
# ./master.py /dev/ttyUSB0 /dev/ttyUSB1

## python system imports
import csv
import threading
import queue
import time
import sys
import os.path
import AEMmailer
from commsMgr import SerialServer

## diskSpaceLimit : in MB, when limit reached, processing halts
diskSpaceLimit = 100 # MB

def diskFree():
    """
    returns value in MB
    """
    st = os.statvfs(os.getcwd())
    free = st.f_bavail * st.f_frsize
    return free / 1000000

class WriterThread(threading.Thread):
    def __init__(self, name, q, lock,fileLockDict, syncTimeFunc, dataDir,stopEvt): #,syncTimeFunc):
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
        @param syncTimeFunc a function that will be called to get the synch time to be used 
        in naming the data files.
        #param stopEvt an event that is set in case this thread has detected 
        a failure contidition
        """
        threading.Thread.__init__(self,daemon=True)
        ## for file name lookup
        self.fileLockDict = fileLockDict
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
        ## setting this event cause the main program to exit
        self.stopEvent = stopEvt

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
            print('* created file:',outFile)

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
        #print('getting filenam',filename)
        self.dictLock.acquire()
        try:
            self.fileLock = self.fileLockDict[filename]
        except KeyError:
            self.fileLock = threading.Lock()
            self.fileLockDict[filename] = self.fileLock
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
                self.q.task_done()
                #print(item)
                if item is None:
                    break
                self.do_work(item)
        except Exception as e:
            print(e)
            print('* thread exiting...')
        finally:
            self.stopEvent.set()

class ReaderThread(threading.Thread):
    def __init__(self, writerq, stopEv, mailerFunc, portT):
        """
        this manages serial server, and its  helper thread
        """
        threading.Thread.__init__(self,daemon=True)
        ## string name of the thread, used for debugging or information messages
        self.server = SerialServer(writerq,portT,stopEv,mailerFunc) 
        ## event to set when disk space runs out we exit
        self.stopEvent = stopEv
        print('* Reader created on port: ',portT)

    def run(self):
        """
        Thread run method. pops the queue, sends the popped thing to be consumed.
        if a None value is popped, the method exits properply and the thread ends.
        """
        try:
            self.server.serve()
        except KeyboardInterrupt:
            self.stopEvent.set()


###### Master Class CODE ##################

class Master:
    def __init__(self, ports = ['/dev/ttyUSB0'], nbThreads=4, dataDir='./DATA'):
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
        ## file lock dire for writer threads
        ## keys: full filename
        ## values: threading.lock object used to guarranty exclusive access to the file for writing
        self.fileLockDict = {}
        ## Directory for data files
        self.dataDir = dataDir
        ## Synchronized work queue
        self.q = queue.Queue()
        ## Semaphore object to be passed to consumer threads for their use
        self.lock = threading.Lock()
        self.stopEvent = threading.Event()
        self.stopEvent.clear()
        self.q = queue.Queue()
        try:
            self.mailQ = queue.Queue()
            self.mailer = AEMmailer.AEMMailer(self.mailQ,self.stopEvent)
        except AEMmailer.NoPasswordException:
            print("* No password provided; no mail will be sent...")
            self.mailer = None
            self.mailQ = None
        if  self.mailer:
             self.mailer.start()
        self.sendMsg("AEM session started!")
        self.startTime = time.strftime('%Y_%m_%d_%H.%M.%S', time.localtime())
        self.createWriterThreads(nbThreads)
        self.createReaderThreads(ports)

    def getSyncTime(self):
        return self.startTime
    
    def sendMsg(self,msg):
        outgoing = time.strftime('%Y %m %d :  %H:%M:%S\n', time.localtime()) + \
                   msg + \
                   '\tDisk space remaining : {} MB'.format(round(diskFree()))
        if self.mailer:
            self.mailQ.put(outgoing)
            #self.mailer.connectAndSend(outgoing)
        print('* mailed:\n'+ outgoing)    

    def createReaderThreads(self,portLis):
        """
        These run the serial server but need to have a helper to take the data coming in and put
        it on the q for the writers...
        """
        self.readerThreads=[]
        for port in portLis:
            reader = ReaderThread(self.q,self.stopEvent,self.sendMsg,port)
            reader.start()
            self.readerThreads.append(reader)
        
    def createWriterThreads(self,num):
        """
        Creates the number of consumer threads according to the argument.
        @param num the number of threads to create
        """
        self.writerThreads = []
        for i in range(num):
            name='WriterThread-' + str(i)
            t = WriterThread(name,self.q,self.lock,self.fileLockDict, self.getSyncTime, self.dataDir,self.stopEvent)
            t.start()
            self.writerThreads.append(t)

    def diskSpaceLimitReached(self):
        res =  diskFree() <= diskSpaceLimit
        if res:
            print('* Disk Space Limit Reached :',diskSpaceLimit,'MB')
            self.sendMsg('Disk Space Limit ' + str(diskSpaceLimit) + ' MB reached!')
        return res

    def run(self):
        """
        called to start a data run, after the consumer threads have been started.
        will display the elapsed time on exit.
        To exit, ctrl-c will be handled properly.
        """
        startTime = time.time()
        try:
            while not(self.diskSpaceLimitReached() and self.stopEvent.wait(60)):
                pass
        except: 
            pass
        finally:
            elapsedTime = round(time.time()-startTime)
            print('* Elapsed Time :', elapsedTime, 'seconds')
            self.sendMsg('Shutting down!\nElapsed Time : ' + str(elapsedTime) + ' seconds.')
            time.sleep(1)
            

if __name__ == '__main__':
    if any(['-h' in sys.argv, '--h' in sys.argv, '--help' in sys.argv]):
        print('Usage: $ ./master.py <ports defaults to /dev/ttyUSB0>'  )
        print('examples;')
        print('Usage: $ ./master.py                               # uses default port /dev/ttyUSB00'  )
        print('Usage: $ ./master.py /dev/ttyUSB0                  # same as previous'  )
        print('Usage: $ ./master.py /dev/ttyACM0 /dev/ttyUSB0     # use these ports')
        print('Note: the AEM board must be running the appropriate software')
        sys.exit(0)

    if len(sys.argv) < 2:
        ## instance of Master class running the entire show!
        master = Master()
    else:
        master = Master(sys.argv[1:])
    master.run()
                 
        
