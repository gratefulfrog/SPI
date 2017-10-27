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
count = 0
class Output(asyncio.Protocol):
    def connection_made(self, transport):
        global startTime
        self.transport = transport
        print('port opened', transport)
        transport.serial.rts = False  # You can manipulate Serial object via transport
        sleep(1)
        transport.write(b'|') #Hello, World!\n')  # Write serial data via transport
        sleep(1)
        transport.write(b'|') #Hello, World!\n')  # Write serial data via transp
        print('Started!')
        startTime=time()

    def data_received(self, data):
        global onGoing
        global current
        global count
        onGoing, current = decodeFromSLIP(data,current)
        if not onGoing:
            count +=1
            if count%10000 == 0:
                print('Structs recevied:',count,'Elapsed seconds:',round(time()-startTime),'current struct:',decodeOSC(bytes(current)))
            onGoing=False
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
    coro = serial_asyncio.create_serial_connection(loop, Output, '/dev/ttyACM0', baudrate=2000000) #115200)
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
