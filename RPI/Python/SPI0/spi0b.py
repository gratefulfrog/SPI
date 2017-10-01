#!/usr/bin/python3

# wiring of SPI 0,0:
"""
Color  : Function : RPI pin (SPI0)  : Smartscope
Yellow : SS       : 24              : D0         NOTE: RPI SPI0-CS1 is  RPI Pin 26
White  : SCK      : 23              : D1
Blue   : MISO     : 21              : D3
Orange : MOSI     : 19              : D2
"""

import spidev,time, sys

# time after each transfer to observe results
pause   = 0  # 0.1 seconds
# SPI config
channel = 0
device  = 0

def go(chars):
    spi = spidev.SpiDev()
    spi.open(channel,device)
    # characters to send
    # out = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'
    index = 0
    init = False
    sendCount=0
    try:
        while True:
            if (chars):
                pass
                """
                outgoing = out[index]
                resp = spi.xfer([ord(outgoing)])  # xfer takes list of ints as arg
                print (sendCount, ': ', [outgoing,  chr(resp[0])])   # and returns a list of ints
                sendCount+=1
                # check the reply w.r.t. previous send 
                if (init and
                    (chr(resp[0]) != out[len(out)-1 if index == 0 else index-1])):
                    print('Error!')
                    raise KeyboardInterrupt
                index=(index+1)%len(out)
                """
            else:
                resp = spi.xfer([index])
                print (sendCount, ': ', [index]+ resp)
                sendCount+=1
                # check reply w.r.t. previous send
                if (init and
                    (resp[0] != (0 if index == 0 else 256-index))):
                    print(index, 'Error!')
                    raise KeyboardInterrupt
                index=(index+1)%256
            init = True
            time.sleep(pause)
    except KeyboardInterrupt:
        print('\nbye...')
    finally:
        spi.close()

if __name__ == '__main__':
    # if arg given, then do letters, else do numbers!
    go(False)
