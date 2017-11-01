#!/usr/local/bin/python3


from time import sleep
import asyncio
import serial_asyncio
from pythonosc import dispatcher,osc_server,osc_message
from OSC import decodeOSC


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

decoder = SlipDecoder()

class Output(asyncio.Protocol):
    def connection_made(self, transport):
        #global startTime
        self.transport = transport
        print('port opened', transport)
        transport.serial.rts = False  # You can manipulate Serial object via transport
        ## handshake
        sleep(1)
        transport.write(b'|')
        sleep(1)
        #transport.write(b'|') #Hello, World!\n')  # Write serial data via transp
        print('Started!')
        #startTime=time()

    def data_received(self, data):
        global onGoing
        global current
        global count
        current = decoder.decodeFromSLIP(data) #decodeFromSLIP(data,current)
        #if not onGoing:
        print(current)
        if current:
            print(decodeOSC(bytes(current)))
               
    def connection_lost(self, exc):
        print('port closed')
        self.transport.loop.stop()

    def pause_writing(self):
        print('pause writing')
        print(self.transport.get_write_buffer_size())

    def resume_writing(self):
        print(self.transport.get_write_buffer_size())
        print('resume writing')

def run():
    l_dispatcher = dispatcher.Dispatcher()
    loop = asyncio.get_event_loop()
    coro = serial_asyncio.create_serial_connection(loop, Output, '/dev/ttyACM0', baudrate=1000000) #115200)
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
