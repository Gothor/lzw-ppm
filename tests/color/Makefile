CC=gcc
CFLAGS=-Wall
PROGNAME=color

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

program: main.o color.o
	@echo $@:
	$(CC) main.o color.o -o $(PROGNAME)

main.o: main.c ../../color.h
	@echo $@:
	$(CC) $(CFLAGS) -c main.c -o main.o

color.o: ../../color.c
	@echo $@:
	$(CC) $(CFLAGS) -c ../../color.c -o color.o

.PHONY: clean

clean:
	$(RM) *.o
