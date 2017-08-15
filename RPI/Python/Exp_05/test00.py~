import spidev
import time

spi = spidev.SpiDev() # create spi object
spi.open(0, 0) # open spi port 0, device (CS) 1

spi.mode(0)
spi.max_speed_hz = 15200

try:
 while True:
 resp = spi.xfer2(['a']) # transfer one byte
 time.sleep(0.1) # sleep for 0.1 seconds
 print(resp)
 #end while
except KeyboardInterrupt: # Ctrl+C pressed, so…
 spi.close() # … close the port before exit
#end try
