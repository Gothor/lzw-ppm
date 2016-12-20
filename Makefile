CC=gcc
CFLAGS=-Wall
PROGNAME=lzw-ppm

ifdef FORWINDOWS
	RM=del /Q
else
	ifeq ($(shell uname), Linux)
		RM=rm -rf
    endif
endif

all: program

debug: CFLAGS += -DDEBUG
debug: program

program: main.o lzw.o bit_writer.o string.o dictionnary.o ppm-exploder.o color.o
	@echo $@:
	$(CC) main.o lzw.o bit_writer.o string.o dictionnary.o ppm-exploder.o color.o -o $(PROGNAME)

main.o: main.c lzw.h ppm-exploder.h
	@echo $@:
	$(CC) $(CFLAGS) -c main.c -o main.o

lzw.o: lzw.c bit_writer.h string.h dictionnary.h
	@echo $@:
	$(CC) $(CFLAGS) -c lzw.c -o lzw.o

bit_writer.o: bit_writer.c
	@echo $@:
	$(CC) $(CFLAGS) -c bit_writer.c -o bit_writer.o

string.o: string.c
	@echo $@:
	$(CC) $(CFLAGS) -c string.c -o string.o

dictionnary.o: dictionnary.c string.h
	@echo $@:
	$(CC) $(CFLAGS) -c dictionnary.c -o dictionnary.o

ppm-exploder.o: ppm-exploder.c color.h
	@echo $@:
	$(CC) $(CFLAGS) -c ppm-exploder.c -o ppm-exploder.o

color.o: color.c
	@echo $@:
	$(CC) $(CFLAGS) -c color.c -o color.o

.PHONY: clean

clean:
	$(RM) *.o
