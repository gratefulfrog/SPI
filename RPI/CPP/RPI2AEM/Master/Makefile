CC=g++
CFLAGS=-lwiringPi -MD
DEPS = master.h app.h  spi.h SPI_anything.h confg.h utilities.h 
OBJ = app.o master.o masterApp.o spi.o utilities.o

%.o: %.cpp 
	$(CC) -c -o $@ $< $(CFLAGS)

all: master

master: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.d

-include $(OBJ:.o=.d)
