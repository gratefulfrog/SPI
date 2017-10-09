#!/usr/bin/python3


"""
# wiring of SPI 0,0:

Color  : Function : RPI pin (SPI0)  : Smartscope
Yellow : SS       : 24              : D0     NOTE: RPI SPI0-CS1 is  RPI Pin 26
White  : SCK      : 23              : D1
Blue   : MISO     : 21              : D3
Orange : MOSI     : 19              : D2
"""

import spidev,time
from struct import pack,unpack

# xfer args: list of bytes,
#            Hz freq of clck,
#            uSec delay before releasing SS

# time after each transfer to observe results
pause   = 0.0000   # seconds
# SPI config
channel        = 0
device         = 0
frequency      = 4000000
afterXferDelay = 30

maxTestFloat = 10.0

s_init_t      = 0b1000  # DEC  8
s_bid_t       = 0b1001  # DEC  9
s_payload_t   = 0b1011  # DEC 11

#s_startWork_t = 0b1100  # DEC 12

# state errors/ 1000 polls
# 229 with 0.01 delays
#

typeDict = { # key=type : value=[ngBytes, formatString, wait time in seconds before next communciation]
    s_init_t       : [9,'<BIf',1], # type 1000, 9 bytes struct, wait after 0.1 s
    s_bid_t        : [9,'<BIf',1], # type 1001, 9 bytes struct, wait after 0.1 s             
    s_payload_t    : [9,'<BIf',0.1]} # type 1010, 9 bytes struct, wait after for 0.1 s

type2NameDict = dict((eval(name),name) for name in ['s_init_t','s_bid_t','s_payload_t'])

nullResponse = [255,0,0.0]  # used as sentinel value

def isNullReturn(responseLis):
    res = all(map(lambda x,y: x==y,responseLis,nullResponse))
    #if res:
    #    print("Null Response Received.")
    return res

########### packing/unpacking routines ##############
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

def unpackStruct( format, packed):
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

########################################################################


def getOutVec(type,nbBytesExpected):
    return [type,] + [1,2]*(nbBytesExpected-1) +  [1,3] 

def show(transferCount,#errorCount,correctedCount,
         currentResponseLis,
         type):
    #if ((transferCount % 10001) == 0):
        print(transferCount,
              ':',
              [round(v,2) for v in currentResponseLis])
              #': Errors :',
              #errorCount,
              #': Corrected :',
              #correctedCount)

def getModV(type):
    modV = maxTestFloat
    if (type == u8_t):
        modV = 256
    elif (type == u32_t):
        modV = pow(2,32)
    return modV
    
        
def tell(init, transferCount,errorCount,currentResponseLis,lastResponseLis,type):
    """
    type is one of:
       s_init_t    
       s_bid_t     
       s_payload_t 
       s_wakeup_t   
    return [init, errorCount, lastResponseLis]
    """
    tempLRL = lastResponseLis
    # the test below is no longer needed since now all responses are structs!
    #if type == s_payload_t:  
    for (t,ind) in map(lambda x,y:(x,y), [u8_t,u32_t,f_t], range(3)):
        lrl = tempLRL if not init else [tempLRL[ind]]
        [i,e,lr] = tell(init,transferCount,errorCount,[currentResponseLis[ind]],lrl,t)
        errorCount += e
        if not init:
            lastResponseLis += lr
        else:
            lastResponseLis[ind] = lr[0]
    return [True, errorCount,lastResponseLis]
        
    takeAway = 0.01 if type == f_t else 1
    modV = getModV(type)
    if (not init) or (round(currentResponseLis[0],2) == 0):
        lastResponseLis = [currentResponseLis[0] - takeAway]
        init = True;
    if type != f_t:
        errorCond = (currentResponseLis[0] != (lastResponseLis[0] + takeAway) % modV)
    else:
        errorCond = abs(currentResponseLis[0] - lastResponseLis[0]) > takeAway+ 0.005
    if errorCond:
        errorCount+=1
        print(transferCount,
              ' : ',
              [currentResponseLis[0],lastResponseLis[0]],
              'Error :',
              errorCount,
              '!********************')
        init=True  # leave this here if not using keyboardInterrupt exception
        raise KeyboardInterrupt
    else:
        lastResponseLis = currentResponseLis
    return [init, errorCount,lastResponseLis]

def masterMsg(rightHalfByte):
    return [(0b1111 & rightHalfByte)<<4]

def isMasterMsg(byte):
    return (byte & (0b1111<<4))

def transferLis(outLis,spi):
    # outLis should be the vector of outgoing bytes, not yet left shifted
    # returns
    # [lis of bytes received, after re-assembly,correctedCount]
    responseLis = []
    correctedCount = 0
    # ignore first reply
    spi.xfer(masterMsg(outLis[0]),frequency,afterXferDelay)
    time.sleep(pause)
    
    for outByte in outLis[1:]:
        responseLis += spi.xfer(masterMsg(outByte),frequency,afterXferDelay)
        while(isMasterMsg(responseLis[-1])):
            correctedCount +=1
            print('Error Corrected :',correctedCount)
            responseLis = responseLis[:-1]
            responseLis += spi.xfer(masterMsg(outByte),frequency,afterXferDelay)
            # here we have a good value in r1
        time.sleep(pause)
    # here we have the response list filled with good values

    resLis = []
    for i in range(0, len(responseLis), 2):
        resLis += [responseLis[i]<<4 | responseLis[i+1]]
    return resLis,correctedCount

def resonseToBeEnqueued(type):
    return type == s_payload_t

def getBid(currentResponseLis):
    return currentResponseLis[1]

bid = [-1]

def doOneCom(type,spi,q,clearTheAir=False,printResults=False):
    global bid
    #print('Processing Query :',type)
    outVec = getOutVec(type,typeDict[type][0])
    transferCount  = 0
    errorCount = 0
    correctedCount = 0
    lastResponseLis = [] 
    init = False
    saidEnq = False
    # first one is ignored, just to clear the air!
    if clearTheAir:
        pass
    #transferLis(outVec,spi)
    moreDataComing = True
    try:
        while moreDataComing:
            #input()
            [currentResponseLis,correctedInc] = transferLis(outVec,spi)
            transferCount+= 1  # note we are now counting transfers not bytes
            correctedCount += correctedInc

            currentResponseLis = unpackStruct(typeDict[type][1],packNbytes(currentResponseLis))
            #currentResponse = bytes2float(currentResponseLis)
            #print(currentResponseLis)
            #input()

            if printResults:
                show(transferCount,#errorCount,correctedCount,
                     currentResponseLis,
                     type)
            #input()
            """
            tell cannot work with query response
            [init, errorCount,lastResponseLis] = tell(init,
                                                      transferCount,
                                                      errorCount,
                                                      currentResponseLis,
                                                      lastResponseLis,
                                                      type)
           
            """
            nullReturn = isNullReturn(currentResponseLis)
            enQResponse = resonseToBeEnqueued(type)
            
            if not nullReturn and enQResponse:
                if not saidEnq:
                    #print('***** Enqueueing...')
                    saidEnq = True
                q.put(list(currentResponseLis) + bid)
            elif type == s_bid_t:
                bid[0] = getBid(currentResponseLis)
                print('BID set :', bid), 
                
            moreDataComing = not nullReturn and enQResponse
                 
    except KeyboardInterrupt:
        print('\nType of Transfers :', type)
        print(  'Nb Transfers      :','{:,}'.format(transferCount).replace(',',' '))
        print(  'Nb Errors         :',errorCount)
        print(  'percent           :',100*errorCount/transferCount)
        print(  'Corrected         :',correctedCount)
        print('\nbye...')
        raise 
    #print('Sleeping :',typeDict[type][2])
    time.sleep(typeDict[type][2])
    

def go(typeLis,q):
    """ opens SPI
    sends each elt in typeLis as per:
    [0] : clearTheAir=True, printResults=True
    [1] : clearTheAir=False, printResults=True
    loop indefinitely:
      [2] : use default values for clearTheAir and printResults
    on exit, closes spi
    """
    spi = spidev.SpiDev()
    spi.open(channel,device)
    try:
        print('Polling :',type2NameDict[typeLis[0]])
        doOneCom(typeLis[0],spi,q,True,False)
        print('Polling :',type2NameDict[typeLis[1]])
        doOneCom(typeLis[1],spi,q,False,False)
        count = 1
        while True:
            if (count%50 == 0):
                print(count, ': Polling :',type2NameDict[typeLis[2]])
            count +=1                
            doOneCom(typeLis[2],spi,q)
    finally:
        spi.close()
