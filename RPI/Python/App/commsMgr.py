#!/usr/bin/python3


"""
# wiring of SPI 0,0:

Color  : Function : RPI pin (SPI0)  : Smartscope
Yellow : SS       : 24              : D0     NOTE: RPI SPI0-CS1 is  RPI Pin 26
White  : SCK      : 23              : D1
Blue   : MISO     : 21              : D3
Orange : MOSI     : 19              : D2
"""

## RPI SPI package
import spidev

## std ptyhon packages
from struct import pack,unpack
from time import sleep, strftime, localtime, time
import os

#### for debugging, will display a heartbeat message after this number of polls
pollDisplayIterations = 100
## qMAxSize : when q reaches this size, data acquistion pauses to let the writer threads
#             clear the q. Acquisiton restarts when q is empty
qMaxSize = 1000000
## diskSpaceLimit : in MB, when limit reached, processing halts
diskSpaceLimit = 100 # MB

# xfer args: list of bytes,
#            Hz freq of clck,
#            uSec delay before releasing SS

## SPI config
## channel on the RPI, there is only one channel
channel        = 0
## SPI clock frequency, in Hz
frequency      = 4000000
## Time in microseconds to wait afer clock stops before releasing slave select
afterXferDelay = 40  # updated to work in 2 board situation

## time after each transfer to observe results
pause   = 0.0000   # seconds

## SPI outgoing polling message bytes
## s_init_t: the init poll 
s_init_t      = 0b1000  # DEC  8
## s_payload_t: the payload poll
s_payload_t   = 0b1011  # DEC 11

class CommsMgr:
    """
    CommsMgr implements error detection in spi by splitting all bytes into upper and lower halves
    master outgoing bytes are:
    * never zero
    * of the form 0bnnnn0000  where at least one n is 1
    slave incoming bytes are:
    * of the form 0b0000nnnn  where there are no constraints on the value of n
    Slave response formats and lengths are given in the typeDict member variable.
    Normal comms protocol:
    * send s_init_t, recover bid values
    * wait 1 second (current value, may change in the future)
    * loop forever:
      * send s_payload_t, recover a payload and enqueue it
      * wait 0.1 second to allow the slave time to refill its work queue
    usage:
    * commsMgr = CommsMgr(aQueue)
    * commsMgr.loop( [ss channels], s_init_t,s_payload_t])
    """
    def __init__(self, queue):
        """
        Instance constructor, handles init for member variables known at init time, others are deferred to runtime init.
        @param queue: the synchronized queue where data will be pushed for the writer threads to process/
        """
        ## Synchonized queue where data from slaves will be pushed
        self.q = queue
        ## Raspberry SPI manager instance 
        self.spi = spidev.SpiDev()
        ## will contain the system time when the slaves are synched
        self.syncTime = 'no_time' # temp valu, same for all devices
        ## contains information per type of outgoing message, i.e. outgoing byte
        ## key: a byte, eg s_init_t or s_payload_t
        ## value: a list of form [nnBytes, formatString, waitTime] where:
        ## nbBytes is how many bytes to exect in response,
        ## formatString is the string to be used for unpacking
        ## waitTime  is how many seconds before next outgoing poll, i.e. how long the slave can work before disturbing it again.
        self.typeDict = { # key=type : value=[nbBytes, formatString, wait time in seconds before next communciation]
                         s_init_t       : [9,'<BIf',1], # type 1000, 9 bytes struct, wait after 0.1 s
                         s_payload_t    : [9,'<BIf',0.1]} # type 1010, 9 bytes struct, wait after for 0.1 s

        ## an inverse dictionary mapping byte to a string name
        ## key: byte as per typeDict
        ## value: the string name of the byte
        self.type2NameDict = dict((eval(name),name) for name in ['s_init_t', 's_payload_t'])
        ## null response from slave is sentinel value used to know when to stop polling for data
        self.nullResponse = [255,0,0.0]  # used as sentinel value

    def initDevices(self, deviceSSChannelLis):
        """
        Initialization call, not part of constructor because these values are not known at
        instanciation time.
        """
        ## list of SS channels provided at runtime
        self.devices = deviceSSChannelLis
        ## dictionary with placeholder values for the Board GUIDs, not yet available
        ## key: SS chanel
        ## value: GUID value, when available, for the board on that channel
        self.bidDict = {device:-1 for device in self.devices}  ## temp values until init call!
        
    def isNullReturn(self,responseLis):
        """
        Returns True if the argument is exactly equal to the self.nullRepsonse
        @param responseLis : a list of values corresponding to a reply from the slave
        @return True if exactly equal to the null response, Fals otherwise
        """
        return all(map(lambda x,y: x==y,responseLis,self.nullResponse))

    ########### packing/unpacking routines ##############
    def packNbytes(self,bytes):
        """ 
        This will return a pack of unsigned bytes, suitable for unpacking into their
        original values.
        the argument: bytes : is a list of any length of 
        unsigned byte values.
        [255, 0, 1, ...]
        returns packed bytes of the form:
        >>> packNbytes(sss)
        b'\x01\n\x00\x00\x00\x00\x00\xc0?'
        """
        return pack('B'*len(bytes),*bytes)

    def unpackStruct(self, format, packed):
        """This will return a list of values
        after unpacking the packed bytes.
        Some formats:
        b/B : signed/unsigned byte 1
        h/H : signed/unsigned short int 2
        i/I : signed/unsigned int 4
        f   : float 4
        d   : double 8
        size and byte order:
        find the system byteorder by examining 
        sys.byteorder
        on Intel and RPI it is 'little'
        @ native order native size, native alignment (needs examination)
        = native order, standard size, no alignment
        </> little/big endian, standard size, no alignment
        !   big endian, standard size, no alignment
        the correct choice is '<' to get little endian, which is what is needed 
        for Arduino and RPI and PC/Intel
        Will return a list of things corresponding to format spec.
        Ex:
        if we have an unsigned byte, unsigned long, float = 9bytes
        >>> unpack('<BIf' packNbytes([<9 numbers that are on [0,255]])
        [uint8_t, uint32_t, f]
        """
        return unpack(format,packed)
    
    ########################################################################

    def getOutVec(self,type,nbBytesExpected):
        """
        returns the vector of outgoing bytes as per the type & number of bytes expected args
        @param type: a tpe byte
        @param nbBytesExpected: the number of bytes expected in reply from slave
        @return the list of outgoing bytes needed to get the proper reply via SPI
        """
        return [type,] + [1,2]*(nbBytesExpected-1) +  [1,3] 

    def show(self,transferCount,currentResponseLis,type):
        """
        Debuggin call used to show information about current comms
        @param transferCound: current number of SPI transfers
        @currentResponseLis: current reply from slave
        @type: current outgoing byte 
        """
        print(transferCount,':', [round(v,2) for v in currentResponseLis])

    def masterMsg(self,rightHalfByte):
        """
        Returns the outgoing byte, encoded onto 2 bytes as per error detection logic
        @param rightHalfByte is the outgoing byte which is in fact only the 4 low order bits
        @return the bits shifted to the high order positions.
        """
        return [(0b1111 & rightHalfByte)<<4]

    def isMasterMsg(self,byte):
        return (byte & (0b1111<<4))

    def checkQ(self):
        if self.q.qsize() > qMaxSize:
            print('Q size reached max, pausing to let writer threads empty it...')
            start = time()
            self.q.join()
            print('Q cleared, polling resumes, pause duration :',
                  round(time()-start),
                  'seconds')
            
    def diskSpaceLimitReached(self):
        st = os.statvfs(os.getcwd())
        free = st.f_bavail * st.f_frsize
        diskFreeMB = free / 1000000
        res =  diskFreeMB <= diskSpaceLimit
        if res:
            print('Disk Space Limit Reached :',diskSpaceLimit,'MB')
        return res
            
    def transferLis(self,outLis):
        """
        This method does the SPI transfers and collects the responses. It also collects error information
        by detecting that the slave has not responded to poll from the master.
        The method handles the SPI logic, ignoring the 1st repsonse, then collecting data into the responseLis
        @param outLis is the list of outgoing bytes to send
        @return resLis, correctedCount: resLis is the list of re-assembled bytes in response, 
        and the corrected count is
        the number of outgoing bytes that the slave did not see on their first send, they are resent until 
        the slave does see them
        """
        responseLis = []
        correctedCount = 0
        # ignore first reply
        self.spi.xfer(self.masterMsg(outLis[0]),frequency,afterXferDelay)
        sleep(pause)
        for outByte in outLis[1:]:
            responseLis += self.spi.xfer(self.masterMsg(outByte),frequency,afterXferDelay)
            while(self.isMasterMsg(responseLis[-1])):
                correctedCount +=1
                print('Error Detected :',correctedCount)
                responseLis = responseLis[:-1]
                responseLis += self.spi.xfer(self.masterMsg(outByte),frequency,afterXferDelay)
            # here we have a good value in r1
            sleep(pause)
        # here we have the response list filled with good values
        
        resLis = []
        # now combine the 1/2 bytes to make real full bytes
        for i in range(0, len(responseLis), 2):
            resLis += [responseLis[i]<<4 | responseLis[i+1]]
        return resLis,correctedCount

    def getBid(self,currentResponseLis):
        """
        Extracts the Board id (GUID) from the currentResponseLis argument
        @param currentRepsonseLis : list of values (re-assembled) received from slave
        @return the Board ID (GUID) value
        """
        return currentResponseLis[1]

    def doOneCom(self,type,device,printResults=False):
        """
        Method does a single poll, obtains response, and enqueues it for the writer threads to handle.
        Manages exceptions to exit properly, returning True if normal exit, and False if exception.
        After doing a poll/repsonse/enqueue it sleeps the amount of time corresponding to the typeDict value
        @param type: outgoing byte
        @param device: SS line to set
        @printResults: True if we want to observe each transfer on stdout, used for debugging
        @return True if normal exit, False if stopped by Keyboard Interrupt (Ctrl-C)
        """
        #print('Processing Query :',type)
        outVec = self.getOutVec(type,self.typeDict[type][0])
        transferCount  = 0
        correctedCount = 0
        moreDataComing = True
        try:
            while moreDataComing:
                [currentResponseLis,correctedInc] = self.transferLis(outVec)
                transferCount+= 1  # note we are now counting transfers not bytes
                correctedCount += correctedInc
                currentResponseLis = self.unpackStruct(self.typeDict[type][1],self.packNbytes(currentResponseLis))

                if printResults:
                    self.show(transferCount, currentResponseLis, type)

                nullReturn = self.isNullReturn(currentResponseLis)
                enQResponse = (type == s_payload_t)
                if nullReturn:
                    pass
                elif enQResponse:
                    self.q.put(list(currentResponseLis) + [self.bidDict[device]])
                elif (type == s_init_t):
                    self.bidDict[device] = self.getBid(currentResponseLis)
                    print('BID',device,'set :', self.bidDict[device]), 

                moreDataComing = not nullReturn and enQResponse

        except KeyboardInterrupt:
            print('\nCommsMgr exiting...')
            return False
        sleep(self.typeDict[type][2])
        return True
    
    def doSynch(self):
        """
        Called to set the Slave Synch time from the current system time
        """
        self.syncTime = strftime("%Y_%m_%d_%H.%M.%S", localtime())

    def getSynchTime(self):
        """
        Gets the current synch time value, warning it may be unset and should be tested, or observed for validity.
        """
        return self.syncTime
    
    def loop(self,typeLis):
        """ 
        The loop method for the CommsMgr, also does initiation of SPI.
        This method does the runtime initialization of the CommsMgr instance, setting the value
        of self.devices.
        After init, the 1st outgoing byte is sent to each device, then in an infinite loop
        the second ougoing byte is sent to each device in turn.
        SPI is managed properly, handling excpetions and exiting correctly in all cases.
        The number or polls per device is maintained during the loops and a heartbeat message is
        periodically displayed to stdout.
        @params typeLis is a list of form [ <SPI devices> <outgoing byte>*] where
        SPI devices is [0] or[1] or [0,1]
        and outgoing bytes should be s_init_t, s_payload_t
        opens SPI
        typeLis[0] is the list of SS Channels, ie [0] or [1], or [0,1]
        sends each elt in typeLis as per:
        [0] : printResults=False
        loop indefinitely:
        [1] : use default values for  printResults
        on exit, closes spi
        """
        self.initDevices(typeLis[0])
        counts=[0,0]
        for device in self.devices:
            try:
                self.spi.open(channel,device)
                print('Polling device',device,':',self.type2NameDict[typeLis[1]])
                if not self.doOneCom(typeLis[1],device,False):
                    self.spi.close()
                    return
                self.doSynch()
                counts[device] +=1
            finally:
                self.spi.close()

        while True:
            for device in self.devices:
                try:
                    self.spi.open(channel,device)
                    if (counts[device]%pollDisplayIterations == 0):
                        self.checkQ()
                        print(counts[device],
                              'Polling device',device,
                              ':',
                              self.type2NameDict[typeLis[2]],
                              'Q size :',
                              self.q.qsize())
                    if (not self.doOneCom(typeLis[2],device)) or self.diskSpaceLimitReached():  ## or no disk space left! note: clearing q takes +/- 20MB of disk!
                        self.spi.close()
                        return
                    counts[device] +=1
                finally:
                    self.spi.close()
