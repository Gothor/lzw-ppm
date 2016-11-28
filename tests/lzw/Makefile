CC=gcc
CFLAGS=-Wall
PROGNAME=lzw

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

program: main.o lzw-ppm.o
	@echo $@:
	$(CC) main.o lzw-ppm.o -o $(PROGNAME)

main.o: main.c ../../lzw-ppm.h
	@echo $@:
	$(CC) $(CFLAGS) -c main.c -o main.o

lzw-ppm.o: ../../lzw-ppm.c
	@echo $@:
	$(CC) $(CFLAGS) -c ../../lzw-ppm.c -o lzw-ppm.o

.PHONY: clean

clean:
	$(RM) *.o
