CC=gcc
CFLAGS=-Wall -DDEBUG
PROGNAME=write_bit_by_bit

ifdef FORWINDOWS
	RM=del /Q
else
	ifeq ($(shell uname), Linux)
		RM=rm -rf
    endif
endif

all: main.o bit_writer.o
	@echo $@:
	$(CC) main.o bit_writer.o -o $(PROGNAME)

main.o: main.c ../../bit_writer.h
	@echo $@:
	$(CC) $(CFLAGS) -c main.c -o main.o

bit_writer.o: ../../bit_writer.c
	@echo $@:
	$(CC) $(CFLAGS) -c ../../bit_writer.c -o bit_writer.o

.PHONY: clean

clean:
	$(RM) *.o