// Maximilian Thompson

#include "constants.h"

char* convert_direction(int d) {
    switch (d) {
    case N:
        return "North";
    case S:
        return "South";
    case E:
        return "East";
    case W:
        return "West";
    default:
        return "ERROR";    
    }
}

char* convert_turn(int t) {
    switch (t) {
    case TLEFT:
        return "left";
    case TSTRAIGHT:
        return "straight";
    case TRIGHT:
        return "right";
    case NO_TURN:
        return "none";
    case WLEFT:
        return "wleft";
    case WSTRAIGHT:
        return "wstright";
    case WRIGHT:
        return "wright";
    default:
        return "ERROR";
    }
}