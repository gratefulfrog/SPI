CC=g++
CFLAGS=-lwiringPi -MD
DEPS = app.h exp.h spi.h SPI_anything.h utilities.h
OBJ = app.o exp.o masterApp.o spi.o utilities.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: exp

exp: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.d

-include $(OBJ:.o=.d)
