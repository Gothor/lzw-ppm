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

all: main.o
	@echo $@:
	$(CC) main.o -o $(PROGNAME)

main.o: main.c
	@echo $@:
	$(CC) $(CFLAGS) -c main.c -o main.o

.PHONY: clean

clean:
	$(RM) *.o