import asyncio
import serial_asyncio
from pythonosc import dispatcher,osc_server

class Output(asyncio.Protocol):
    def connection_made(self, transport):
        self.transport = transport
        print('port opened', transport)
        transport.serial.rts = False  # You can manipulate Serial object via transport
        transport.write(b'Hello, World!\n')  # Write serial data via transport

    def data_received(self, data):
        print('data received', repr(data))
        if b'\n' in data:
            self.transport.close()

    def connection_lost(self, exc):
        print('port closed')
        self.transport.loop.stop()

    def pause_writing(self):
        print('pause writing')
        print(self.transport.get_write_buffer_size())

    def resume_writing(self):
        print(self.transport.get_write_buffer_size())
        print('resume writing')

loop = asyncio.get_event_loop()
coro = serial_asyncio.create_serial_connection(loop, Output, '/dev/ttyUSB0', baudrate=1000000)
loop.run_until_complete(coro)

dispatcher = dispatcher.Dispatcher()
# This will print all parameters to stdout.

dispatcher.map("/test", print)

server_address = (0,0)
server = osc_server.AsyncIOOSCUDPServer(server_address, dispatcher, loop)
server.serve()
loop.run_forever()
loop.close()
