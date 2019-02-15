# Maximilian Thompson
# 
# Makefile for the drivers problem in project 3.

all: drivers

drivers: carThread.c carThread.h constants.c constants.h intersection.c
	gcc -g -Wall -pthread *.c -o drivers

clean:
	rm -f drivers *.o
