#!/usr/bin/python3

"""
**** AEM Results ****
With interrupts on slave (Slave2650_sendLong):
Nb Transfers  :  330 187
Nb Errors     :  0
percent       :  0.0
Corrected     :  0

Without interrupts on slave, no loop delay (Slave2650_sendLong):
Nb Transfers  :  1 482 073
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
from struct import pack

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

def bytes2unint32(byteVec):
    return int.from_bytes(byteVec,byteorder='little', signed=False)

def bytes2float(byteVec):
    return unpack('f',byteVec)[0]

def floatBytes2ByteVec (bytes):
    return pack('4B', *bytes)

def doFloat(listOfCBytes):
    return bytes2float(floatBytes2ByteVec(listOfCBytes))


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

def go():
    spi = spidev.SpiDev()
    spi.open(channel,device)
    outVec = [0b1000,1,2,1,2,1,2,1,3]
    transferCount  = 0
    errorCount = 0
    correctedCount = 0
    lastResponse = -1
    init = False
    maxLong = pow(2,32)-1
    try:
        while True:
            [currentResponseLis,correctedInc] = transferLis(outVec,spi)
            currentResponse = doFloat(currentResponseLis)
            #print(currentResponseLis)
            #print(currentResponse)
            #input()
            transferCount+= 1  # note we are now counting transfers not bytes
            correctedCount += correctedInc
            if not init:
                lastResponse = -.01 if currentResponse==0 else currentResponse-0.01
                init = True;
            if ((transferCount % 10000) == 0):
                print(transferCount,
                      ':',
                      currentResponse,
                      '  ' if  currentResponse<10 else
                      ' ' if currentResponse<100 else
                      '',
                      ': Errors :',
                      errorCount,
                      ': Corrected :',
                      correctedCount)
            
            # check relative to previous
            if (abs(currentResponse- lastResponse) > 0.001):
                errorCount+=1
                print(transferCount,
                      ' : ',
                      [currentResponse,lastResponse],
                      'Error :',
                      errorCount,
                      '!********************')
                #input()
            else:
                
                lastResponse = currentResponse
            
    except KeyboardInterrupt:
        print('\nNb Transfers  : ','{:,}'.format(transferCount).replace(',',' '))
        print(  'Nb Errors     : ',errorCount)
        print(  'percent       : ',100*errorCount/transferCount)
        print(  'Corrected     : ',correctedCount)
        print('\nbye...')
    finally:
        spi.close()

if __name__ == '__main__':
    # if arg given, then do letters, else do numbers!
    go()
