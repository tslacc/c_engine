#include <stdio.h>
#include <time.h>
static void mainLoop(void){
	while(1){
		break;
	}
}
unsigned long int programStartTime=0;
int main()
{
	struct timespec bT;
	timespec_get(&bT, TIME_UTC);
	printf("progst = %lu\n", programStartTime);
	
	programStartTime = bT.tv_nsec;
	printf("progst = %lu\n", programStartTime);
	mainLoop();
	return 0;
}

