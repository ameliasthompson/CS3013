// Maximilian Thompson

#include "normDist.h"

double norm_dist(double mean, double stddev) {
    return (sqrt(-2.0 * log(drand48())) * cos(2.0 * PI * drand48())) * stddev + mean;
}