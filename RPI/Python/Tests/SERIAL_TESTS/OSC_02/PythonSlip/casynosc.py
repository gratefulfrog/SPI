#!/usr/bin/python3

import serial
from time import sleep
from OSC import decodeOSC


class OSCProcessor():
    def __init__(self):
        self.fDict = {'/i'  :self.doSingleton,
                      '/iif':self.doStruct}

    def doSingleton(self,singletonLis):
        print(singletonLis[0])
        
    def doStruct(self,structLis):
        adcChid = structLis[0]
        timeStamp = structLis[1]
        value = round(structLis[2],3)
        print(adcChid,timeStamp,value)
    
    def dispatchOSCList(self,oscLis):
        self.fDict[oscLis[0]](oscLis[2:])

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
    def __init__(self, portT, bd = 115200, to = None):
        self.port        = portT
        self.stopEvent   = False
        self.baudrate    = bd
        self.timeout     = to
        self.slipDecoder = SlipDecoder()
        self.dispatcher  = OSCProcessor()
        
    def serve(self):
        with serial.Serial(port = self.port, baudrate= self.baudrate, timeout = self.timeout) as ser:
            sleep(1)
            # first clear anything on the incoming port
            ser.timeout = 0
            while ser.read():
                pass
            ser.timeout = self.timeout

            # now give the handshake!
            sleep(1)
            ser.write(b'|')
            sleep(1)
            print('Started... ctrl-C to exit')
            
            while True:
                try:
                    if self.stopEvent:
                        print('\nexting...')
                        break
                    res = self.slipDecoder.decodeFromSLIP(ser.read(16)) # 16 bytes
                    if res:
                        self.dispatcher.dispatchOSCList(decodeOSC(bytes(res)))
                except KeyboardInterrupt:
                    self.stopEvent = True
            
if __name__ == '__main__':
    import sys
    if len(sys.argv) == 2:
        server = SerialSever(sys.argv[1])
    else:
        server = SerialServer('/dev/ttyACM0')
    server.serve()
