#!/usr/bin/python3

from time import sleep
from struct import pack,unpack
import serial
import queue

import threading
import itertools

#### for debugging, will display a heartbeat message after this number of polls
pollDisplayIterations = 2000000 #= +/-1 hour # 32000 # = 1 min
#pollDisplayIterations = 32000 # = 1 min 

USB_FULL_SPEED = 12100000
M_SPEED        =  1000000

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
    """
    This helper thread unburdens the server from the processing of the incoming data.
    It is needed to prevent a bottleneck at the receiving end of the serial port.
    """
    def __init__(self, inq, outq,mailer):
        threading.Thread.__init__(self)
        ## inq: where the serial server puts data to be processed and forwarded
        ## outq: where the helper thread will place the processed data
        ## mailer: a function that will email messages off to the world
        self.inQ  = inq
        self.init = False
        self.comms = CommsMgr(outq,mailerFunc=mailer)
        self.bid  = None

    def do_work(self,thing):
        """
        take a thing of the incoming queue, process it.
        the thing is n blocks of 9 bytes, so we break it down into 9s and then process each nine
        using pack/unpack.
        The 1st pack contains the BID and is processed differently.
        """
        iter9 = grouper(thing,9)
        #print(list(iter9))
        try:
            while True:
                b = packNbytes(next(iter9))
                v = list(unpackStruct('<BIf',b))
                #print(v)
                if not self.init:
                    #print('Init:', v)
                    self.comms.bidFunc(v[1])
                    sleep(1)
                    self.init = True
                else:
                    self.comms.structFunc(v)
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
            print('* thread exiting...')
        finally:
            self.comms.outQ.put(None)
            self.inQ.task_done()
            print('\n* Input Reader exiting...')

class CommsMgr:
    """
    part of the helper thread, used to process the incoming data structs
    and forward them as needed 
    """
    def __init__(self,
                 outq,
                 mailerFunc,
                 countLim=pollDisplayIterations,
                 sv = True):   ##<! show values to the world  or not 
        self.outQ       = outq
        self.mailerFunc = mailerFunc
        self.count      = 0
        self.modCount   = countLim
        self.showVals   = sv
        
    def bidFunc(self, bid):
        """
        process the incoming bid, and save it for future use
        """
        self.bid=bid
        self.mailerFunc('Started : BID: ' + str(self.bid))

    def structFunc(self,s):
        """
        process incoming struct, append bid, put it on outQ
        if counts are enough, tell the world!
        """
        #print('putting:', s+[self.bid])
        self.outQ.put(s + [self.bid])
        self.count +=1
        if self.showVals and self.count%self.modCount == 0:
            self.showWhatWePut()

    def showWhatWePut(self):
        """
        form a message and send it off
        """
        outgoing = str(self.bid)     + \
                   ' : Structs Received : ' + \
                   str(self.count)  
        self.mailerFunc(outgoing)


    def decodeADCCHN(self, val):
        """
        decode the byte containing ADC id and Channel id into the values
        return: adc, channel
        """
        return (val >> 4) & 0b1111, val & 0b1111

class SerialServer():
    """
    Implent the serving of the serial port,
    reads blocks of 900 bytes, i.e. 100 structs and enqueues them for the helper thread to process
    """
    def __init__(self, writerq, portT, stopEv , mailerFunc, bd = USB_FULL_SPEED, to = None):
        self.port        = portT
        self.stopEvent   = stopEv
        self.baudrate    = bd
        self.timeout     = to
        self.outQ        = queue.Queue()
        self.processor   = ReadInputThread(self.outQ,writerq,mailerFunc)  ##<! the helper thread
        self.processor.start()
        
    def serve(self):
        """
        clears any incoming stuff, gives the AEM handshake, and serves until shut down by someone
        """
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
                        print('\n* waiting for processor to finish...')
                        self.outQ.join()
                        break
                    self.outQ.put(ser.read(900))
                except KeyboardInterrupt:
                    self.stopEvent.set()
