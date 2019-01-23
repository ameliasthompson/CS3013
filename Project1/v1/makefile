# Maximilian Thompson
#
# Makefile for project 1 phase 2.

DEBUG = -g -Wall -D_DEBUG

all: mc1 seymour

mc1: mc1.o util.o option.o
	gcc $(DEBUG) mc1.o util.o option.o -o mc1

mc1.o: mc1.c
	gcc -c $(DEBUG) mc1.c

util.o: util.c util.h
	gcc -c $(DEBUG) util.c
	
option.o: option.c option.h
	gcc -c $(DEBUG) option.c

seymour: seymour.c
	gcc $(DEBUG) seymour.c -o seymour

clean:
	rm -f *.o mc1 seymour
