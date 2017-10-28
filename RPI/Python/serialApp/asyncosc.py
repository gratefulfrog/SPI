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
modCount = 101
def doBid(bidLis):
    print('BID:',bidLis[1])

def doStruct(structLis):
    global count
    global modCount
    adc = structLis[1] >> 4 & 0b1111
    channel = structLis[1] & 0b1111
    timeStamp = structLis[2]
    value = round(structLis[3],3)
    count +=1
    #print('toto')
    if count%modCount == 0:                
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
        onGoing, current = decodeFromSLIP(data,current)
        if not onGoing:
            count +=1
            #if count%modCount == 0:
                #print('Structs recevied:',count,'Elapsed seconds:',round(time()-startTime),'current struct:',decodeOSC(bytes(current)))
            dispatchOSCList(decodeOSC(bytes(current)))
            current=[]
               
    def connection_lost(self, exc):
        print('port closed')
        self.transport.loop.stop()

    def pause_writing(self):
        print('pause writing')
        print(self.transport.get_write_buffer_size())

    def resume_writing(self):
        print(self.transport.get_write_buffer_size())
        print('resume writing')

    
SLIP_END = 0o300
SLIP_ESC = 0o333
SLIP_ESC_END = 0o334
SLIP_ESC_ESC = 0o335

class ProtocolError(Exception):
    pass

def getSerialByte(serialFD):
    next(serialFD)

def decodeFromSLIP(bytes, dataBuffer = []):
    """
    arguments are bytes to decode and current decoded list if any
    return onGoing, Current
    where onGoing is True if the Current is not complete
    and onGoing is False if the Current is complete
    """
    serialFD=iter(bytes)
    try:
        while True:
            serialByte = next(serialFD)
            if serialByte is None:
                raise ProtocolError
            elif serialByte == SLIP_END:
                if len(dataBuffer) > 0:
                    return False,dataBuffer
            elif serialByte == SLIP_ESC:
                serialByte = next(serialFD)
                if serialByte is None:
                    raise ProtocolError
                elif serialByte == SLIP_ESC_END:
                    dataBuffer.append(SLIP_END)
                elif serialByte == SLIP_ESC_ESC:
                    dataBuffer.append(SLIP_ESC)
                else:
                    raise ProtocolError
            else:
                dataBuffer.append(serialByte)
    except StopIteration:
        return True, dataBuffer

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
