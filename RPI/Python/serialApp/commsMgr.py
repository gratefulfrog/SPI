#!/usr/bin/python3

from time import sleep,strftime,localtime
from struct import pack,unpack
import serial

import threading
import itertools

#### for debugging, will display a heartbeat message after this number of polls
pollDisplayIterations = 200 #40000 # = 1/hour

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
    def __init__(self, inq, outq):
        threading.Thread.__init__(self)
        ## work q, source of data to be written to files
        self.inQ  = inq
        self.init = False
        self.comms = CommsMgr(outq)
        self.bid  = None

    def do_work(self,thing):
        iter9 = grouper(thing,9)
        try:
            b = packNbytes(next(iter9))
            v = [round(x,3) for x in unpackStruct('<BIf',b)]
            if not self.init:
                print('Init:', v)
                self.comms.bidFunc(v[1])
                sleep(5)
                self.init = True
            else:
                self.comms.structFunc(v+[self.bid])
        except StopIteration:
            return
            
    def run(self):
        """
        Thread run method. pops the queue, sends the popped thing to be consumed.
        if a None value is popped, the method exits properply and the thread ends.
        """
        try:
            while True:
                item = self.inQ.get()
                #print(item)
                if item is None:
                    break
                self.do_work(item)
                self.inQ.task_done()
        except Exception as e:
            print(e)
            print('thread exiting...')
        finally:
            self.comms.outQ.put(None)
            self.inQ.task_done()
            print('\nInput Reader exiting...')

def nullMail(msg):
    print('mailed:', msg)    

class CommsMgr:
    def __init__(self,
                 outq,
                 mailerFunc = nullMail,
                 countLim=pollDisplayIterations,
                 sv = True,
                 sa = False):
        self.outQ = outq
        self.mailerFunc = mailerFunc
        self.count =  self.timeSum = self.timeCount = self.lastTimeStamp = 0
        self.modCount = countLim
        self.showVals= sv
        self.showAvg = sa
        self.initTimeStamps = False
        
    def bidFunc(self, bid):
        self.bid=bid
        startTime = strftime('%Y_%m_%d_%H.%M.%S', localtime())
        print('Started!\n',startTime,'\nBID:',self.bid)

    def structFunc(self,s):
        self.outQ.put(s)
        self.count +=1
        if self.showVals and self.count%self.modCount == 0:
            self.showWhatWePut()

    def showWhatWePut(self):
        outgoing = str(self.bid) + \
                   ' : Poll count: ' + \
                   str(self.count) 

        self.mailerFunc(outgoing)


    def decodeADCCHN(self, val):
        return (val >> 4) & 0b1111, val & 0b1111

class SerialServer():
    def __init__(self, writerq, portT, stopEv , mailerFunc,bd = 1000000, to = None):
        self.outQ        = outq
        self.port        = portT
        self.stopEvent   = stopEv
        self.baudrate    = bd
        self.timeout     = to
        self.outQ        = queue.Queue()
        self.processor   = ReadInputThread(self.outQ,writerq)
        self.processor.start()
        
    def serve(self):
        with serial.Serial(port = self.port,
                           baudrate= self.baudrate,
                           timeout = self.timeout) as ser:
            sleep(1)
            # first clear anything on the incoming port
            ser.timeout = 0
            while ser.read():
                pass
            ser.timeout = self.timeout

            # now give the handshake!
            sleep(1)
            ser.write(b'|')
            
            # now go for it!
            while True:
                try:
                    if self.stopEvent.is_set():
                        self.outQ.put(None)
                        print('\nwaiting for processor to finish...')
                        self.outQ.join()
                        break
                    self.q.put(ser.read(900))
                except KeyboardInterrupt:
                    self.stopEvent.set()
                                    
"""
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


if __name__ == '__main__':
    import sys, threading
    stopEv = threading.Event()
    stopEv.clear()
    if len(sys.argv) == 2:
        server = SerialSever(stopEv,sys.argv[1])
    else:
        server = SerialServer(stopEv)
    server.serve()
"""
