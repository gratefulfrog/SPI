#!/usr/bin/python3


"""
# wiring of SPI 0,0:

Color  : Function : RPI pin (SPI0)  : Smartscope
Yellow : SS       : 24              : D0     NOTE: RPI SPI0-CS1 is  RPI Pin 26
White  : SCK      : 23              : D1
Blue   : MISO     : 21              : D3
Orange : MOSI     : 19              : D2
"""

import spidev
from struct import pack,unpack
from time import sleep, strftime, localtime

#### for debugging, will display a heartbeat message after this number of polls
pollDisplayIterations = 100

# xfer args: list of bytes,
#            Hz freq of clck,
#            uSec delay before releasing SS

# SPI config
channel        = 0
devices        = [0]  # i.e. SS channels, this could be [0], [1], or [0,1]
frequency      = 4000000
afterXferDelay = 30


# time after each transfer to observe results
pause   = 0.0000   # seconds

# SPI outgoing polling message bytes
s_init_t      = 0b1000  # DEC  8
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
      * wait 0.1 second
    usage:
    * commsMgr = CommsMgr(aQueue)
    * commsMgr.loop([s_init_t,s_payload_t])
    """
    def __init__(self, queue):
        self.q = queue
        self.spi = spidev.SpiDev()
        self.devices = devices
        self.bidDict = {device:-1 for device in self.devices}  ## temp values until init call!
        self.syncTime = 'no_time' # temp valu, same for all devices
        self.typeDict = { # key=type : value=[ngBytes, formatString, wait time in seconds before next communciation]
                         s_init_t       : [9,'<BIf',1], # type 1000, 9 bytes struct, wait after 0.1 s
                         s_payload_t    : [9,'<BIf',0.1]} # type 1010, 9 bytes struct, wait after for 0.1 s

        self.type2NameDict = dict((eval(name),name) for name in ['s_init_t', 's_payload_t'])
        self.nullResponse = [255,0,0.0]  # used as sentinel value

    def isNullReturn(self,responseLis):
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
        return [type,] + [1,2]*(nbBytesExpected-1) +  [1,3] 

    def show(self,transferCount,currentResponseLis,type):
        print(transferCount,':', [round(v,2) for v in currentResponseLis])

    def masterMsg(self,rightHalfByte):
        return [(0b1111 & rightHalfByte)<<4]

    def isMasterMsg(self,byte):
        return (byte & (0b1111<<4))

    def transferLis(self,outLis):
        # outLis should be the vector of outgoing bytes, not yet left shifted
        # returns
        # [lis of bytes received, after re-assembly,correctedCount]
        responseLis = []
        correctedCount = 0
        # ignore first reply
        self.spi.xfer(self.masterMsg(outLis[0]),frequency,afterXferDelay)
        sleep(pause)
        for outByte in outLis[1:]:
            responseLis += self.spi.xfer(self.masterMsg(outByte),frequency,afterXferDelay)
            while(self.isMasterMsg(responseLis[-1])):
                correctedCount +=1
                print('Error Corrected :',correctedCount)
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
        return currentResponseLis[1]

    def doOneCom(self,type,device,printResults=False):
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
            print('\nType of Transfers :', type)
            print(  'Nb Transfers      :','{:,}'.format(transferCount).replace(',',' '))
            print(  'Corrected         :',correctedCount)
            print('\nbye...')
            raise KeyboardInterrupt
        sleep(self.typeDict[type][2])
    
    def doSynch(self,):
        self.syncTime = strftime("%Y_%m_%d_%H.%M.%S", localtime())

    def getSynchTime(self):
        return self.syncTime
    
    def loop(self,typeLis):
        """ opens SPI
        sends each elt in typeLis as per:
        [0] : printResults=False
        loop indefinitely:
        [1] : use default values for  printResults
        on exit, closes spi
        """
        counts=[0,0]
        for device in self.devices:
            try:
                self.spi.open(channel,device)
                print('Polling device',device,':',self.type2NameDict[typeLis[0]])
                self.doOneCom(typeLis[0],device,False)
                self.doSynch()
                counts[device] +=1
            finally:
                self.spi.close()

        while True:
            for device in self.devices:
                try:
                    self.spi.open(channel,device)
                    if (counts[device]%pollDisplayIterations == 0):
                        print(counts[device],
                              'Polling device',device,':',
                              self.type2NameDict[typeLis[1]])
                    self.doOneCom(typeLis[1],device)
                    counts[device] +=1                
                finally:
                    self.spi.close()
