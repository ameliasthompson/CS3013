# Maximilian Thompson
#
# Makefile for project 1 phase 2.

DEBUG = -g -Wall -D_DEBUG
LIB = -pthread

all: mc2 seymour

mc2: mc2.o util.o option.o cprocess.o waitthread.o
	gcc $(DEBUG) $(LIB) mc2.o util.o option.o cprocess.o waitthread.o -o mc2

mc2.o: mc2.c
	gcc -c $(DEBUG) $(LIB) mc2.c

util.o: util.c util.h
	gcc -c $(DEBUG) $(LIB) util.c
	
option.o: option.c option.h
	gcc -c $(DEBUG) $(LIB) option.c

cprocess.o: cprocess.c cprocess.h
	gcc -c $(DEBUG) $(LIB) cprocess.c

waitthread.o: waitthread.c waitthread.h
	gcc -c $(DEBUG) $(LIB) waitthread.c

seymour: seymour.c
	gcc $(DEBUG) $(LIB) seymour.c -o seymour

clean:
	rm -f *.o mc2 seymour
