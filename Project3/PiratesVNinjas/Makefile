# Maximilian Thompson
# 
# Makefile for the pirates vs ninjas problem in project 3.

all: pvn

pvn: department.c department.h human.c human.h normDist.c normDist.h pvn.c
	gcc -g -Wall -D_DEBUG -pthread *.c -o pvn -lm -lrt

clean:
	rm -f pvn *.o