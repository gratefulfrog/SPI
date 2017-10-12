# This is the makefile for the the RPI master application
# it uses:
## g++ as the compiler
## automatic dependency generation (-include $(OBJ:.o=.d) and -MD flag
#  -lwiringPi to include the RPI wiring libraries, only available on RPI 
#
# use target: all to build applicatin
# use target: test to build csv writer test, and uncomment -include line at end
# 

CC=g++
CFLAGS=-lwiringPi -MD
CTESTFLAGS=-MD
OBJ = app.o fileMgr.o master.o masterApp.o spi.o utilities.o
TESTOBJ = fileTest.o fileMgr.o utilities.o

all: master

test: fileTest

master: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

fileTest: $(TESTOBJ)
	$(CC) -o $@ $^ $(CTESTFLAGS)

%.o: %.cpp 
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.d

-include $(OBJ:.o=.d)
# only for the disk writer testing
#-include $(TESTOBJ:.o=.d)

