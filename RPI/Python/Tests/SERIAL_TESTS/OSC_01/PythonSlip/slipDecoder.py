#!/usr/local/bin/python3

import serial
from time import sleep

class SerialServer():
    def __init__(self, portT, bd = 1000000):
        self.port        = portT
        self.stopEvent   = False
        self.baudrate    = bd
        self.timeout     = None
        self.slipDecoder = SlipDecoder()
        
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
            while True:
                try:
                    if self.stopEvent:
                        break
                    res = self.slipDecoder.decodeFromSLIP(ser.read(50))
                    print(res)
                except KeyboardInterrupt:
                    self.stopEvent = True
                    #return

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

"""
if __name__ == '__main__':
    # create some test data
    OSCMsgs =[[192,47, 116, 101, 115, 116, 0, 0, 0, 44, 105, 0, 0, 0, 0, 0, 4,192],  # ['/test', ',i', 4]
              [192,47, 116, 101, 115, 116, 0, 0, 0, 44, 105, 0, 0, 0, 0, 1, 23,192], # ['/test', ',i', 279]
              [192,47, 116, 101, 115, 116, 47, 48, 0, 44, 98, 98, 102, 0, 0, 0, 0, 0, 0, 0, 1, 21, 0, 0, 0, 0, 0, 0, 4, 0, 8, 8, 8, 66, 242, 0, 0,192],
              # ['/test/0', ',bbf', b'\x15', b'\x00\x08\x08\x08', 121.0]
              [192,47, 116, 101, 115, 116, 47, 48, 0, 44, 105, 105, 102, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 250, 66, 200, 61, 116,192]]
              # ['/test/0', ',iif', 12, 250, 100.12002563476562]

    byteStream = b''
    for m in OSCMsgs:
        byteStream += bytes(m)
    print(byteStream)
    
    # get a decoder
    dec = SlipDecoder()
    res = dec.decodeFromSLIP(byteStream)
    while res:
        print(res)
        res = dec.decodeFromSLIP(byteStream)

 """
if __name__ == '__main__':
    import sys
    defaultPort = '/dev/ttyACM0'
    if len(sys.argv)==2:
        s = SerialServer(sys.argv[1])
    else: 
        s = SerialServer(defaultPort)

    s.serve()


    
