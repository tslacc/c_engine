#include <time.h>
#include <stdio.h>
void uniGetTimespec(struct timespec *dest){
#ifdef __linux__
#if _POSIX_C_SOURCE >= 199309L
	clock_gettime(CLOCK_MONOTONIC_COARSE, dest);
#endif
#else 
	timespec_get(dest, TIME_UTC);
#endif
}
