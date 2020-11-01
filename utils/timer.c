#include "timer.h"

struct timespec startTimer() { // Start nanosecond timer
	struct timespec startTime;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startTime);

	return startTime;
}

long int endTimer(struct timespec startTime) { // End nanosecond timer and return elapsed time in milliseconds
	struct timespec endTime;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endTime);
	long diffInNanos = (endTime.tv_sec - startTime.tv_sec) * (long)1e9 + (endTime.tv_nsec - startTime.tv_nsec);

	return diffInNanos;
}