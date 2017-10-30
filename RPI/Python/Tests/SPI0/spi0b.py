#!/usr/bin/python3

"""
**** AEM Results ****
With interrupts on slave (Slave2650_b):
Nb Sends  :  50 082 004 (50 million)
Nb Errors :  10
percent   :  1.996725210916081e-05

Without itnerrupts on slave, no loop delay (Slave2650_c):
Nb Sends  :  50 078 939 (50 millio)
Nb Errors :  12
percent   :  2.396216900681542e-05


**** Iteaduino Results ****
With interrupts on slave (Slave2650_b):
5V logic:
Nb Sends  :  5 049 149
Nb Errors :  0
percent   :  0.0

3.3V Logic:
Nb Sends  :  5 106 485
Nb Errors :  0
percent   :  0.0

Without itnerrupts on slave, no loop delay (Slave2650_c):
5V logic:
Nb Sends  :  5 059 091
Nb Errors :  0
percent   :  0.0

3.3V Logic:
Nb Sends  :  5 284 112
Nb Errors :  0
percent   :  0.0
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

# time after each transfer to observe results
pause   = 0.0000   # 0.1 seconds
# SPI config
channel = 0
device  = 0

def go():
    spi = spidev.SpiDev()
    spi.open(channel,device)
    index = 0
    init = False
    sendCount=0
    errorCount=0
    try:
        while True:
            resp = spi.xfer([index],1000000,2)
            # xfer args: list of bytes,
            #            Hz freq of clck,
            #            uSec delay before releasing SS
            time.sleep(pause)
            if ((sendCount % 10000) == 0):
                print (sendCount, ': ', [index]+ resp)
           
            #print (sendCount, ': ', [index]+ resp)
            sendCount+=1
            # check reply w.r.t. previous send
            if (init and
                (resp[0] != (0 if index == 0 else 256-index))):
                errorCount+=1
                print(sendCount,
                      ' : ',
                      [index, resp[0]],
                      'Error :',
                      errorCount,
                      '!********************')
                init = False
                #raise KeyboardInterrupt
            else:
                #input()
                index=(index+1)%256
                init = True
    except KeyboardInterrupt:
        print('\nNb Sends  : ',sendCount)
        print(  'Nb Errors : ',errorCount)
        print(  'percent   : ',100*errorCount/sendCount)
        print('\nbye...')
    finally:
        spi.close()

if __name__ == '__main__':
    # if arg given, then do letters, else do numbers!
    go()
