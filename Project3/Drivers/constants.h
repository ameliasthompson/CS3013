// Maximilian Thompson

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Constants.
#define NUM_CARS    20
#define SLEEPTIME   250000
#define RECYCLETIME 1

// Turn enumeration.
#define NO_TURN    -1
// Want to turn.
#define WLEFT       0
#define WSTRAIGHT   1
#define WRIGHT      2
// Are turning.
#define TLEFT       3
#define TSTRAIGHT   4
#define TRIGHT      5

// Quadrant enumeration.
#define NW          0
#define NE          1
#define SW          2
#define SE          3

// Queue enumeration.
#define N           0
#define S           1
#define E           2
#define W           3

// Helper functions.
char* convert_direction(int d);
char* convert_turn(int t);

#endif
