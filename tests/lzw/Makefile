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

program: main.o lzw.o bit_writer.o dictionnary.o string.o
	@echo $@:
	$(CC) main.o lzw.o bit_writer.o string.o dictionnary.o -o $(PROGNAME)

main.o: main.c ../../lzw.h
	@echo $@:
	$(CC) $(CFLAGS) -c main.c -o main.o

lzw.o: ../../lzw.c ../../bit_writer.h ../../string.h
	@echo $@:
	$(CC) $(CFLAGS) -c ../../lzw.c -o lzw.o

bit_writer.o: ../../bit_writer.c
	@echo $@:
	$(CC) $(CFLAGS) -c ../../bit_writer.c -o bit_writer.o

dictionnary.o: ../../dictionnary.c ../../string.h
	@echo $@:
	$(CC) $(CFLAGS) -c ../../dictionnary.c -o dictionnary.o

string.o: ../../string.c
	@echo $@:
	$(CC) $(CFLAGS) -c ../../string.c -o string.o

.PHONY: clean

clean:
	$(RM) *.o
