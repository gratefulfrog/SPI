#!/usr/bin/python3

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
pause   = 0.0   # 0.1 seconds
# SPI config
channel = 0
device  = 0

def go():
    spi = spidev.SpiDev()
    spi.open(channel,device)
    index = 0
    init = False
    sendCount=0
    try:
        while True:
            # iteaduino values :
            # resp = spi.xfer([index],1000000, 2)
            # AEM values:
            resp = spi.xfer([index],1000000, 2)
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
                print([index, resp[0]], 'Error!')
                raise KeyboardInterrupt
            #input()
            index=(index+1)%256
            init = True
    except KeyboardInterrupt:
        print('\nbye...')
    finally:
        spi.close()

if __name__ == '__main__':
    # if arg given, then do letters, else do numbers!
    go()
