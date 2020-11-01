#ifndef TIMER_H
#define TIMER_H

#include <time.h>

struct timespec startTimer(); // Start nanosecond timer

long int endTimer(struct timespec); // End nanosecond timer and return elapsed time in nanoseconds

#endif