CC=gcc
CFLAGS=-Wall -DDEBUG
PROGNAME=explode_ppm

ifdef FORWINDOWS
	RM=del /Q
else
	ifeq ($(shell uname), Linux)
		RM=rm -rf
    endif
endif

all: main.o ppm-exploder.o
	@echo $@:
	$(CC) main.o ppm-exploder.o -o $(PROGNAME)

main.o: main.c ../../ppm-exploder.h
	@echo $@:
	$(CC) $(CFLAGS) -c main.c -o main.o

ppm-exploder.o: ../../ppm-exploder.c
	@echo $@:
	$(CC) $(CFLAGS) -c ../../ppm-exploder.c -o ppm-exploder.o

.PHONY: clean

clean:
	$(RM) *.o
