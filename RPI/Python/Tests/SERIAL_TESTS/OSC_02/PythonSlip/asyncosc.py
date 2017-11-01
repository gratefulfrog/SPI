#!/usr/local/bin/python3


from time import sleep,time
import asyncio
import serial_asyncio
from pythonosc import dispatcher,osc_server, osc_message

from OSC import decodeOSC

from slipDecoder import SlipDecoder


decoder = SlipDecoder()

class Output(asyncio.Protocol):
    def connection_made(self, transport):
        global startTime
        self.transport = transport
        print('port opened', transport)
        transport.serial.rts = False  # You can manipulate Serial object via transport
        #handshake
        sleep(1)
        transport.write(b'|') 
        sleep(1)
        print('Started!')

    def data_received(self, data):
        current = decoder.decodeFromSLIP(bytes(data))
        print(current)
        print(list(map(lambda v: round(v,3) if type(v)==float else v,decodeOSC(bytes(current)))))
                       
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
