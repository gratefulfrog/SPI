#!/usr/bin/python3

"""
**** AEM Results ****
With interrupts on slave (Slave2650_b1):

Without interrupts on slave, no loop delay (Slave2650_c1):


"""

# wiring of SPI 0,0:
"""
Color  : Function : RPI pin (SPI0)  : Smartscope
Yellow : SS       : 24              : D0     NOTE: RPI SPI0-CS1 is  RPI Pin 26
White  : SCK      : 23              : D1
Blue   : MISO     : 21              : D3
Orange : MOSI     : 19              : D2
"""

import spidev,time, sys

# xfer args: list of bytes,
#            Hz freq of clck,
#            uSec delay before releasing SS

# time after each transfer to observe results
pause   = 0.0000   # seconds
# SPI config
channel = 0
device  = 0
sendCount = 0

def masterMsg(rightHalfByte):
    return [(0b1111 & rightHalfByte)<<4]

def isMasterMsg(byte):
    return (byte & (0b1111<<4))

def go():
    spi = spidev.SpiDev()
    spi.open(channel,device)
    outVec = [0b1000,1,2]
    sendCount  = 0
    errorCount = 0
    lastResponse = -1
    currentRepsonse = 0
    init = False
    try:
        while True:
            outIndex = 0
            # send the type and ignore left over value of SPDR 
            spi.xfer(masterMsg(outVec[outIndex]),1000000,2)

            time.sleep(pause)
            outIndex +=1
            [r1] = spi.xfer(masterMsg(outVec[outIndex]),1000000,2)
            while(isMasterMsg(r1)):
                  [r1] = spi.xfer(masterMsg(outVec[outIndex]),1000000,2)
            # here we have a good value in r1

            time.sleep(pause)
            outIndex +=1
            [r2] = spi.xfer(masterMsg(outVec[outIndex]),1000000,2)
            while(isMasterMsg(r2)):
                  [r2] = spi.xfer(masterMsg(outVec[outIndex]),1000000,2)
            # here we have a good value in r2
                  
            time.sleep(pause)
            sendCount+=1
            currentResponse = r1<<4 | r2
            if not init:
                lastResponse = 255 if currentResponse==0 else currentResponse-1
                init = True;
            if ((sendCount % 10000) == 0):
                print(sendCount,' : ', currentResponse)
            # check relative to previous
            if (currentResponse!= (lastResponse +1)%256):
                errorCount+=1
                print(sendCount,
                      ' : ',
                      [currentResponse,lastResponse],
                      'Error :',
                      errorCount,
                      '!********************')
                input()
            else:
                lastResponse = r1<<4 | r2
            
    except KeyboardInterrupt:
        print('\nNb Sends  : ','{:,}'.format(sendCount).replace(',',' '))
        print(  'Nb Errors : ',errorCount)
        print(  'percent   : ',100*errorCount/sendCount)
        print('\nbye...')
    finally:
        spi.close()

if __name__ == '__main__':
    # if arg given, then do letters, else do numbers!
    go()
