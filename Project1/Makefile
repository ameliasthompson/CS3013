# Maximilian Thompson
#
# Makefile for project 1.

DEBUG = -g -Wall -D_DEBUG

all: mc0 mc1 mc2 seymour0 seymour1 seymour2

# v0

mc0: v0/mc0.c
	cd v0; gcc $(DEBUG) mc0.c -o mc0

seymour0: v0/seymour.c
	cd v0; gcc $(DEBUG) seymour.c -o seymour

# v1

mc1: v1/mc1.c v1/util.c v1/util.h v1/option.c v1/option.h
	cd v1; gcc $(DEBUG) mc1.c util.c option.c -o mc1

seymour1: v1/seymour.c
	cd v1; gcc $(DEBUG) seymour.c -o seymour

# v2

mc2: v2/mc2.c v2/util.c v2/util.h v2/option.c v2/option.h v2/cprocess.c v2/cprocess.h v2/waitthread.c v2/waitthread.h
	cd v2; gcc $(DEBUG) -pthread mc2.c util.c option.c cprocess.c waitthread.c -o mc2

seymour2: v2/seymour.c
	cd v2; gcc $(DEBUG) seymour.c -o seymour

# clean

clean:
	cd v0; rm -f *.o mc0 seymour
	cd v1; rm -f *.o mc1 seymour
	cd v2; rm -f *.o mc2 seymour
