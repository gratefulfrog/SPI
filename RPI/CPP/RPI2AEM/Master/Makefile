CC=g++
CFLAGS=-lwiringPi -MD
CTESTFLAGS=-MD
#DEPS = master.h app.h confg.h spi.h SPI_anything.h utilities.h 
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
