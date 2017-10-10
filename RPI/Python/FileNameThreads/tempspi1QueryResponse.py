#!/usr/bin/python3


"""
# wiring of SPI 0,0:

Color  : Function : RPI pin (SPI0)  : Smartscope
Yellow : SS       : 24              : D0     NOTE: RPI SPI0-CS1 is  RPI Pin 26
White  : SCK      : 23              : D1
Blue   : MISO     : 21              : D3
Orange : MOSI     : 19              : D2
"""
from time import strftime, localtime

syncTime = strftime("%Y_%m _%d_%H.%M.%S", localtime())
