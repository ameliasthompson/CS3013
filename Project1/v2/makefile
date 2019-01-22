# Maximilian Thompson
#
# Makefile for project 1 phase 2.

DEBUG = -g -Wall -D_DEBUG

all: mc2 seymour

mc2: mc2.o util.o option.o cprocess.o
	gcc $(DEBUG) mc2.o util.o option.o cprocess.o -o mc2

mc2.o: mc2.c
	gcc -c $(DEBUG) mc2.c

util.o: util.c util.h
	gcc -c $(DEBUG) util.c
	
option.o: option.c option.h
	gcc -c $(DEBUG) option.c

cprocess.o: cprocess.c cprocess.h
	gcc -c $(DEBUG) cprocess.c

seymour: seymour.c
	gcc $(DEBUG) seymour.c -o seymour

clean:
	rm -f *.o mc2 seymour
