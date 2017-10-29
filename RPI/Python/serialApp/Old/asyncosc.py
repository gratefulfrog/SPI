#!/usr/local/bin/python3


from time import sleep,time
import asyncio
import serial_asyncio
from pythonosc import dispatcher,osc_server

from pythonosc import osc_message

from OSC import decodeOSC

onGoing = False
current= []
startTime = 0 # seconds
count = -1
modCount = 1
carry = None
carryBytes = bytes([])
timeSum=0
timeCount =0
lastTimeStamp = 0

showVals= True
showAvg = False

def doBid(bidLis):
    print('BID:',bidLis[1])

def doStruct(structLis):
    global count
    global timeSum
    global timeCount
    global lastTimeStamp
    adc = structLis[1] >> 4 & 0b1111
    channel = structLis[1] & 0b1111
    timeStamp = structLis[2]
    value = round(structLis[3],3)
    count +=1
    if showAvg:
        if channel == 0:
            timeSum+=timeStamp-lastTimeStamp
            timeCount+=1
            lastTimeStamp = timeStamp
        avg = round(timeSum/timeCount)
        print('Time Between reads of 0 channel:',avg,'microseconds')
    if showVals and count%modCount == 0:
        print('ADC: {}\tChannel: {}\tTimeStamp: {}\tValue: {}'.format(adc,channel,timeStamp,value))
    
def dispatchOSCList(oscLis):
    global count
    fDict={'/i':doBid,
           '/iif':doStruct}
    count +=1
    #if count%modCount == 0:                
    #print(oscLis)
    #return
    fDict[oscLis[0]](oscLis[1:])
    

class Output(asyncio.Protocol):
    def connection_made(self, transport):
        global startTime
        self.transport = transport
        print('port opened', transport)
        transport.serial.rts = False  # You can manipulate Serial object via transport
        sleep(1)
        transport.write(b'|')
        sleep(1)
        transport.write(b'|')
        print('Started!')
        startTime=time()

    def data_received(self, data):
        global onGoing
        global current
        global count
        global modCount
        global carry
        global carryBytes
        onGoing, current,carry,carryBytes = decodeFromSLIP(data,current,carry,carryBytes)
        if not onGoing and current:
            #print(decodeOSC(bytes(current)))
            #print(carry)
            dispatchOSCList(decodeOSC(bytes(current)))
            current=[]
            #carry=None
               
    def connection_lost(self, exc):
        print('port closed')
        self.transport.loop.stop()

    def pause_writing(self):
        print('pause writing')
        print(self.transport.get_write_buffer_size())

    def resume_writing(self):
        print(self.transport.get_write_buffer_size())
        print('resume writing')

    
SLIP_END     = 0o300   # 192 0xC0
SLIP_ESC     = 0o333   # 219 0xDB
SLIP_ESC_END = 0o334   # 220 0xDC
SLIP_ESC_ESC = 0o335   # 221 0xDD

d= b'\xc0/iif\x00\x00\x00\x00,iif\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x1b$;\xa6@\x00\xc0'
bad=b'\xc0/iif\x00\x00\x00\x00,iif\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00%\xb4@\x0f\xdb\xdc\x00\xc0'

"""
carryBytes b'\xc0/iif\x00\x00\x00\x00,iif\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00(,?\xb2\xc8\xdb\xdc\xc0\xc0/iif\x00\x00\x00\x00,iif\x00\x00\x00\x00\x00\x00\x00\x06\x00\x00*\x98?M\n\x00\xc0'
bytesIn b'\xc0/iif\x00\x00\x00\x00,iif\x00\x00\x00\x00\x00\x00\x00\x07\x00\x00-\x04?C\x00\x00\xc0'

"""

class ProtocolError(Exception):
    pass

def getSerialByte(serialFD):
    next(serialFD)

def decodeFromSLIP(bytesIn, dataBuffer = [], carry=None, carryBytes=bytes([])):
    """
    arguments are bytes to decode and current decoded list if any, and last carrybyte SLIP_ESC 
    return onGoing, Current, carry 
    where onGoing is True if the Current is not complete
    and onGoing is False if the Current is complete
    """
    incoming = carryBytes+bytesIn
    if carry:
        incoming = carry + incoming
    serialFD=iter(incoming)
    try:        
        while True:
            serialByte = next(serialFD)  ## could raise StopIteration, so carry better be right!
            if serialByte == SLIP_END:
                if len(dataBuffer) > 0:  ## true if this is not a 'start byte'
                    return False,dataBuffer,None,bytes(serialFD)
            elif serialByte == SLIP_ESC:
                carry = SLIP_ESC
                serialByte = next(serialFD)  ## could raise  StopIteration, with new carry
                if serialByte == SLIP_ESC_END:
                    dataBuffer.append(SLIP_END)
                elif serialByte == SLIP_ESC_ESC:
                    dataBuffer.append(SLIP_ESC)
                else:
                    raise ProtocolError

            else:
                carry=None # serialByte  ## in case of future StopIteration exception
                dataBuffer.append(serialByte)
    except StopIteration:
        return True, dataBuffer, carry, bytes(serialFD) ## ongoing

def run():
    l_dispatcher = dispatcher.Dispatcher()
    loop = asyncio.get_event_loop()
    coro = serial_asyncio.create_serial_connection(loop, Output, '/dev/ttyUSB0', baudrate=2000000) #115200)
    loop.run_until_complete(coro)
    
    # This will print all parameters to stdout.
    # doesn't work
    l_dispatcher.map('test',print)
    
    server_address = ("127.0.0.1",5005)
    server = osc_server.AsyncIOOSCUDPServer(server_address, l_dispatcher, loop)
    server.serve()
    loop.run_forever()
    loop.close()
    
if __name__ == '__main__':
    run()
