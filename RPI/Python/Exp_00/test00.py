import spidev
import time

spi = spidev.SpiDev() # create spi object
spi.open(0, 0) # open spi port 0, device (CS) 1

spi.mode = 0
spi.max_speed_hz = 15200

def getNextLetter(index):
    return index+1 if ((index+1) < (123))  else 0x61

def run():
    index = 0x61
    try:
        while True:
            print str(unichr(index))
            index = getNextLetter(index)            
            resp = spi.xfer2([index]) # transfer one byte
            time.sleep(0.1) # sleep for 0.1 seconds
            print(str(unichr(resp[0])))
    except KeyboardInterrupt: # Ctrl+C pressed, so
        spi.close() # close the port before exit
  
