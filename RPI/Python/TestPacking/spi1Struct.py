#!/usr/bin/python3

"""
**** AEM Results ****
With interrupts on slave (Slave2650_sendFloat):
Nb Transfers  :  7 881 286
Nb Errors     :  0
percent       :  0.0
Corrected     :  0

Without interrupts on slave, no loop delay (Slave2650_sendFloat):
Nb Transfers  :  9 020 970
Nb Errors     :  0
percent       :  0.0
Corrected     :  0

"""

# wiring of SPI 0,0:
"""
Color  : Function : RPI pin (SPI0)  : Smartscope
Yellow : SS       : 24              : D0     NOTE: RPI SPI0-CS1 is  RPI Pin 26
White  : SCK      : 23              : D1
Blue   : MISO     : 21              : D3
Orange : MOSI     : 19              : D2
"""

import spidev,time
from struct import pack,unpack
from packBytes import packNbytes, unpackStruct

# xfer args: list of bytes,
#            Hz freq of clck,
#            uSec delay before releasing SS

# time after each transfer to observe results
pause   = 0.0000   # seconds
# SPI config
channel        = 0
device         = 0
frequency      = 1000000
afterXferDelay = 2

maxTestFloat = 10.0

u8_t  = 0b1000
u32_t = 0b1001
f_t   = 0b1010
s_t   = 0b1011

typeDict = { # key=type : value=[ngBytes, formatString]
    u8_t  : [1,'<B'],   # type 1000, 1 byte, i.e. one unsigned byte  i.e. uint8_t
    u32_t : [4,'<I'],   # type 1001, 4 bytes, i.e. one unsigned Long i.e. uint32_t
    f_t   : [4,'<f'],   # type 1010, 4 bytes, i.e. one float         i.e. float
    s_t   : [9,'<BIf']} # type 1011, 9 bytes struct,                 i.e. uint8_t,uint32_t float

def getOutVec(type,nbBytesExpected):
    return [type,] + [1,2]*(nbBytesExpected-1) +  [1,3]

def show(transferCount,errorCount,correctedCount,currentResponseLis):
    if ((transferCount % 10001) == 0):
        print(transferCount,
              ':',
              [round(v,2) for v in currentRepsonseLis],
              ': Errors :',
              errorCount,
              ': Corrected :',
              correctedCount)

def tell(errorCount,currentResponseLis,lastResponseLis,type):
    """
    type is one of 'u8','u32','f', 's'    
    return [init, errorCount, lastResponseLis]
    """
    if type == 's':
        for (t,ind) in map(lambda x,y: ['u8','u32','f'], range(3)):
            [i,e,lr] = tell(errorCount,currentResponseLis,lastResponseLis[ind],v)
            init = init || i
            errorCount += e
            lastRepsonse[ind] = lr
        return [init, errorCount,lastResponseLis]
        
    takeAway = 0.01 if type =='f' else 1
    if (type == u8_t):
        modV = 256
    elif (type == u32_t):
        modV = pow(2,32)
    else:
        modV = maxTestFloat 
    if not init:
        lastResponseLis = [currentResponseLis[0] - takeAway]
        init = True;
    if (currentResponse[0] != (lastResponseLis[0] + takeAway) % modV):
        errorCount+=1
        print(transferCount,
              ' : ',
              [currentResponse[0],lastResponseLis[0]],
              'Error :',
              errorCount,
              '!********************')
        #input()
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
    #print(responseLis)
    for i in range(0, len(responseLis), 2):
        resLis += [responseLis[i]<<4 | responseLis[i+1]]
    return resLis,correctedCount


def go(type):
    spi = spidev.SpiDev()
    spi.open(channel,device)
    #outVec = [0b1000,1,2,1,2,1,2,1,3]
    outVec = getOutVec(type,typeDict[type][0])
    transferCount  = 0
    errorCount = 0
    correctedCount = 0
    lastResponse = -0.01
    init = False
    try:
        while True:
            [currentResponseLis,correctedInc] = transferLis(outVec,spi)
            transferCount+= 1  # note we are now counting transfers not bytes
            correctedCount += correctedInc

            currentResponseLis = unpackStruct(typeDict[type][1],packNbytes(byteList))
            #currentResponse = bytes2float(currentResponseLis)
            #print(currentResponseLis)
            #print(currentResponse)
            #input()

            show(transferCount,errorCount,correctedCount,currentResponseLis)
            [init, errorCount,lastResponseLis] = tell(errorCount,currentResponseLis,lastResponseLis,type)
            
    except KeyboardInterrupt:
        print('\nNb Transfers  : ','{:,}'.format(transferCount).replace(',',' '))
        print(  'Nb Errors     : ',errorCount)
        print(  'percent       : ',100*errorCount/transferCount)
        print(  'Corrected     : ',correctedCount)
        print('\nbye...')
    finally:
        spi.close()

import sys

if __name__ == '__main__':
    # if arg given, then do letters, else do numbers!
    go(eval(sys.argv[1]))


################ OBSOLETE STUFF not yet deleted #################
    
def bytes2unint32(byteVec):
    """
    returns single value.
    """
    return unpackStruct('<I',packNbytes(byteVec))[0]

def bytes2float(listOfCBytes):
    """
    returns single value.
    """
    return  unpackStruct('<f',packNbytes(listOfCBytes))[0]

def bytes2Struct(listOfCBytes):
    """ 
    returns list of values!
    """
    return  unpackStruct( '<BIf',packNbytes(byteList))
