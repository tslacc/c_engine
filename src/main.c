#define _POSIX_C_SOURCE 199309L
#include "uniTime.h"
#include <stdio.h>
#include <time.h>


#define NS 1000000000.0

struct timespec programStartTime;
struct timespec lastFrameTime;
struct timespec subFrameTime;
struct timespec frameTimeTarget;
unsigned long long int frameTimeTarget_nsec;
const unsigned int FPS = 60;
const unsigned int FRAMES = FPS*3;
static void timespecCopy(struct timespec *const dest, const struct timespec *const src){
	dest->tv_sec = src->tv_sec;
	dest->tv_nsec = src->tv_nsec;
}
static float timespecFloat(const struct timespec *const src){
	return src->tv_sec+src->tv_nsec/NS;
}
static unsigned long long int timespec_nsec(const struct timespec *const src){
	return src->tv_sec*NS+src->tv_nsec;
}
static unsigned long long int timespecDiff_nsec(const struct timespec *const lo, const struct timespec *const hi){
	/*printf("src is %f %f\n", timespecFloat(lo), timespecFloat(hi));
	printf("lo is %lu, %lu\n", lo->tv_sec, lo->tv_nsec);
	printf("hi is %lu, %lu\n", hi->tv_sec, hi->tv_nsec);
	printf("Calcing timespec diff\n\t%lu sec %lu nsec\n", (hi->tv_sec-lo->tv_sec), (hi->tv_sec-lo->tv_nsec));
	*///safety check, just in case lo and hi are mismatched...
	if(lo->tv_sec > hi->tv_sec || lo->tv_sec == (hi->tv_sec && lo->tv_nsec > hi->tv_nsec)) return 0;
	//printf("\tExpected result is %llu\n\t%lf\n", result, result/NS);
	return (unsigned long long int)((hi->tv_sec-lo->tv_sec)*NS+hi->tv_nsec-lo->tv_nsec);
}
static void timespecDiff_struct(struct timespec *const dest, const struct timespec *const lo, const struct timespec *const hi){
	dest->tv_sec = 0;
	dest->tv_nsec = 0;
	dest->tv_sec = hi->tv_sec-lo->tv_sec;
	if(hi->tv_nsec<lo->tv_nsec){
		dest->tv_sec-=1;
		dest->tv_nsec+=NS;
	}
	dest->tv_nsec = dest->tv_nsec+hi->tv_nsec-lo->tv_nsec;
}
unsigned long int brickedCycles = 0;
// Blocks until the time is valid for another tick.
// Returns the time delta of the frame.
static unsigned long long int frame_sync(void){
	uniGetTimespec(&subFrameTime);
	static struct timespec sft_diff;
	static struct timespec wait_time;
	
	timespecDiff_struct(&sft_diff, &lastFrameTime, &subFrameTime);
	unsigned long long int sft_diff_nsec = timespec_nsec(&sft_diff);
	
	if(sft_diff_nsec<frameTimeTarget_nsec){
		timespecDiff_struct(&wait_time, &sft_diff, &frameTimeTarget);
		nanosleep(&wait_time, NULL);
		brickedCycles++;
	}
	uniGetTimespec(&lastFrameTime);
	return sft_diff_nsec+timespec_nsec(&wait_time);
}
static void mainLoop(void){
	unsigned int i = 0;
	struct timespec _bT;
	struct timespec _eT;
	timespec_get(&_bT, TIME_UTC);
	while(i < FRAMES){
		unsigned long long int dT = frame_sync();
		timespec_get(&_eT, TIME_UTC);
		//printf("frame time: %lf\n", dT/NS);
		timespecCopy(&_bT, &_eT);
		i++;
	}
}
int main()
{
	timespecFloat(NULL);
	frameTimeTarget.tv_nsec = NS/FPS;
	frameTimeTarget_nsec = frameTimeTarget.tv_nsec;
	uniGetTimespec(&programStartTime);
	timespecCopy(&lastFrameTime, &programStartTime);
	mainLoop();
	unsigned long long int diff = timespecDiff_nsec(&programStartTime, &lastFrameTime);
	printf("%lu cycles spent to do %u total frames, taking %lf sec\n", brickedCycles, FRAMES, diff/NS);
	printf("Expected time is %f\n\ttotal error %f\n\tframe error %f\n\tpercentile error %f\n", frameTimeTarget_nsec*FRAMES/NS,\
		diff/NS-frameTimeTarget_nsec*FRAMES/NS,\
		(diff/NS-frameTimeTarget_nsec*FRAMES/NS)/FRAMES,\
		(diff/NS-frameTimeTarget_nsec*FRAMES/NS)/(frameTimeTarget_nsec*FRAMES/NS));
	return 0;
}
//Notes:
//It takes 22 microseconds to do timespec_get, so don't do it too often.
//Every frame takes 16666 microseconds.
