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

program: main.o
	@echo $@:
	$(CC) main.o -o $(PROGNAME)

main.o: main.c
	@echo $@:
	$(CC) $(CFLAGS) -c main.c -o main.o

lzw-ppm.o: lzw-ppm.c
	@echo $@:
	$(CC) $(CFLAGS) -c lzw-ppm.c -o lzw-ppm.o

ppm-parser.o: ppm-parser.c
	@echo $@:
	$(CC) $(CFLAGS) -c ppm-parser.c -o ppm-parser.o

.PHONY: clean program

clean:
	$(RM) *.o
