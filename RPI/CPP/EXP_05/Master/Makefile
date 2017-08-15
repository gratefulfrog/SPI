CC=g++
CFLAGS=-lwiringPi
DEPS = app.h exp.h SPI_anything.h 
OBJ = exp.o app.o masterApp.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: exp

exp: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o
