import spidev
import time

spi = spidev.SpiDev() # create spi object

letters = 'abcdefghijklmnopqrstuvwxyz'

def getNextLetter(index):
    return (index+1) % len(letters)

def initSPI():
    spi.open(0, 0) # open spi port 0, device (CS) 0
    spi.mode = 0
    spi.max_speed_hz = 15200
    

def run():
    initSPI()
    index = 0
    sendCount = 0
    print ("Master")
    spi.xfer([ord(letters[index])]) # transfer one byte
    try:
        while True:
            print ("Send: " +str(sendCount))
            sendCount+=1
            print ('Sent: ' + letters[index])
            index = getNextLetter(index)            
            resp = spi.xfer([ord(letters[index])]) # transfer one byte
            print('Received: ' + chr(resp[0]))
            time.sleep(1) # sleep for 0.1 seconds
    except KeyboardInterrupt: # Ctrl+C pressed, so
        spi.close() # close the port before exit
  
