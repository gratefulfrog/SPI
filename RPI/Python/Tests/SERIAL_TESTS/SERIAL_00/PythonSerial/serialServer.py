#!/usr/bin/python3


from time import sleep
from struct import pack,unpack
import serial

import threading
import queue
import time
import sys
import itertools

def packNbytes(bytes):
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

def unpackStruct(format, packed):
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

def grouper(iterable, n, fillvalue=None):
        "Collect data into fixed-length chunks or blocks"
        # grouper('ABCDEFG', 3, 'x') --> ABC DEF Gxx"
        args = [iter(iterable)] * n
        return itertools.zip_longest(*args, fillvalue=fillvalue)

class ReadInputThread(threading.Thread):
    def __init__(self, q):
        threading.Thread.__init__(self)
        ## work q, source of data to be written to files
        self.q = q
        self.init = False
        self.count = self.noks = 0

    def do_work(self,thing):
        iter9 = grouper(thing,9)
        try:
            b = packNbytes(next(iter9))
            v = [round(x,3) for x in unpackStruct('<BIf',b)]
            if not self.init:
                print('Init:', v)
                sleep(5)
                self.init = True
            else:
                ok = (v[0]*10 == v[1]) and (v[2] *1000 == v[0])
                if not ok:
                    print(v)
                self.noks = self.noks if ok else self.noks+1
                if not ok or ((self.count%100)==0):                 
                    print('Count :',self.count, '\tNok count :',self.noks, '\tQ size :',self.q.qsize())
            self.count +=1
            #sleep(0.01)
        except StopIteration:
            return
            
    def run(self):
        """
        Thread run method. pops the queue, sends the popped thing to be consumed.
        if a None value is popped, the method exits properply and the thread ends.
        """
        try:
            while True:
                item = self.q.get()
                #print(item)
                if item is None:
                    break
                self.do_work(item)
                self.q.task_done()
        except Exception as e:
            print(e)
            print('thread exiting...')
        finally:
            self.q.task_done()
            print('\nInput Reader exiting...')
                


class SerialServer():
    def __init__(self, q, portT ='/dev/ttyACM0' ,bd = 1000000, to = None):
        self.port        = portT
        self.baudrate    = bd
        self.timeout     = to
        self.q = q
    
    def serve(self):
        with serial.Serial(port = self.port,
                           baudrate= self.baudrate,
                           timeout = self.timeout) as ser:
            sleep(1)
            ser.timeout = 0 # no wait
            while ser.read():
                pass
            ser.timeout = self.timeout
            
            # now give the handshake!
            sleep(1)
            ser.write(b'|')
            # now go for it!
            while True:
                try:
                    self.q.put(ser.read(900))
                except KeyboardInterrupt:
                    self.q.put(None)
                    print('\nwaiting for processor to finish...')
                    self.q.join()
                    return
                          
    
def run(p='/dev/ttyACM0'):
    q= queue.Queue()
    processor = ReadInputThread(q)
    processor.start()
    server=SerialServer(q,p)
    server.serve()
    
if __name__ == '__main__':
    p= '/dev/ttyACM0'
    if len(sys.argv) == 2:
        p= sys.argv[1]
    run(p)
