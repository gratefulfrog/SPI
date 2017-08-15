CC=g++
CFLAGS=-lwiringPi
DEPS = master.h app.h  SPI_anything.h confg.h utilities.h 
OBJ = master.o app.o masterApp.o utilities.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: master

exp: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o
