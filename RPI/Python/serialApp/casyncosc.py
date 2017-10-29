#!/usr/local/bin/python3

import serial
from time import sleep,time,strftime,localtime
from OSC import decodeOSC

def nullMail(msg):
    print('mailed:', msg)

class CommsMgr:
    def __init__(self,q, mailerFunc = nullMail, countLim=1, sv = True, sa = False):
        self.q = q
        self.mailerFunct = mailerFunc
        self.count =  self.timeSum = self.timeCount = self.lastTimeStamp = 0
        self.modCount = countLim
        self.showVals= sv
        self.showAvg = sa
        self.initTimeStamps = False
        
    def bidFunc(self, bid):
        self.bid=bid
        startTime = strftime('%Y_%m_%d_%H.%M.%S', localtime())
        print('Started!\n',startTime,'\nBID:',self.bid)

    def structFunc(self,adcChid,timeStamp,value):
        self.q.put([adcChid,timeStamp,value,self.bid])
        
    def showWhatWePut(self,adcChid,timeStamp,value):
        adc,channel =  self.decodeADCCHN(adcChid)
        self.count +=1
        if self.showAvg:
            if channel == 0:
                if not self.initTimeStamps:
                    self.lastTimeStamp = timeStamp
                    self.initTimeStamps = True
                else:
                    self.timeSum += timeStamp-self.lastTimeStamp
                    self.timeCount += 1
                    self.lastTimeStamp = timeStamp
                    avg = round(self.timeSum/self.timeCount)
                    print('Time Between reads of 0 channel:',avg,'microseconds')
        if self.showVals and self.count%self.modCount == 0:
            print('count: {}\tADC: {}\tChannel: {}\tTimeStamp: {}\tValue: {}'.format(self.count,adc,channel,timeStamp,value))

    def decodeADCCHN(self, val):
        return (val >> 4) & 0b1111, val & 0b1111

class OSCProcessor():
    def __init__(self, q, mailerFunc):
        self.fDict = {'/i'  :self.doBid,
                      '/iif':self.doStruct}
        self.commsMgr = CommsMgr(q,mailerFunc)

    def doBid(self,bidLis):
        self.commsMgr.bidFunc(bidLis[1])
        sleep(2)
        
    def doStruct(self,structLis):
        adcChid = structLis[1]
        timeStamp = structLis[2]
        value = round(structLis[3],3)
        self.commsMgr.structFunc(adcChid,timeStamp,value)
    
    def dispatchOSCList(self,oscLis):
        self.fDict[oscLis[0]](oscLis[1:])

class ProtocolError(Exception):
    pass

class SlipDecoder():
    SLIP_END     = 0o300   # 192 0xC0
    SLIP_ESC     = 0o333   # 219 0xDB
    SLIP_ESC_END = 0o334   # 220 0xDC
    SLIP_ESC_ESC = 0o335   # 221 0xDD
    
    def __init__(self):
        self.dataBuffer = []
        self.carry      = bytes([])
        self.carryBytes = bytes([])
        self.reset      = False
        
    def resetForNewBuffer(self):
        self.dataBuffer = []
        self.carry = bytes([])
        self.reset = False

    def decodeFromSLIP(self,bytesIn): 
        """
        arguments are bytes to decode
        return decode value or None if not available
        """
        if self.reset:
            self.resetForNewBuffer()

        serialFD=iter(self.carry + self.carryBytes + bytesIn)
        try:        
            while True:
                serialByte = next(serialFD)           ## could raise StopIteration, so carry better be right!
                if serialByte == SlipDecoder.SLIP_END:
                    if len(self.dataBuffer) > 0:       ## true if this is not a 'start byte'
                        self.carryBytes=bytes(serialFD)
                        self.reset = True
                        return self.dataBuffer         ## exit the while loop HERE only!
                elif serialByte == SlipDecoder.SLIP_ESC:
                    self.carry = bytes([SlipDecoder.SLIP_ESC])
                    serialByte = next(serialFD)        ## could raise  StopIteration, with new carry
                    if serialByte == SlipDecoder.SLIP_ESC_END:
                        self.dataBuffer.append(SlipDecoder.SLIP_END)
                    elif serialByte == SlipDecoder.SLIP_ESC_ESC:
                        self.dataBuffer.append(SlipDecoder.SLIP_ESC)
                    else:
                        raise ProtocolError
                else:
                    self.carry=bytes([])    ## in case of  StopIteration exception at top of loop
                    self.dataBuffer.append(serialByte)
        except StopIteration:
            self.carryBytes = bytes(serialFD)
            return None  # here reset is false

class SerialServer():
    def __init__(self, portT, stopEv, q , mailerFunc,bd = 2000000, to = None):
        self.port        = portT
        self.stopEvent   = stopEv
        self.baudrate    = bd
        self.timeout     = to
        self.slipDecoder = SlipDecoder()
        self.dispatcher  = OSCProcessor(q,mailerFunc)
        
    def serve(self):
        with serial.Serial(port = self.port, baudrate= self.baudrate, timeout = self.timeout) as ser:
            #handshake!
            sleep(1)
            ser.write(b'|')
            while True:
                try:
                    if self.stopEvent.is_set():
                        break
                    res = self.slipDecoder.decodeFromSLIP(ser.read(512))
                    if res:
                        self.dispatcher.dispatchOSCList(decodeOSC(bytes(res)))
                except KeyboardInterrupt:
                    self.stopEvent.set()
                    return
            
if __name__ == '__main__':
    import sys, threading
    stopEv = threading.Event()
    stopEv.clear()
    if len(sys.argv) == 2:
        server = SerialSever(stopEv,sys.argv[1])
    else:
        server = SerialServer(stopEv)
    server.serve()
