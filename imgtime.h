#include <sys/time.h>
#include <unistd.h>

struct timeval start;
struct timeval stop;
int time_flag;
int time_use;

#define TIME_START {\
	time_flag = 1;\
	gettimeofday(&start,0);\
}

#define TIME_STOP {\
	gettimeofday(&stop,0);\
	if(time_flag==1)\
	{\
		time_use = (stop.tv_sec-start.tv_sec);\
		time_use = (time_use>=0)?time_use:(0xFFFFFFFF+time_use);\
		time_use = time_use*1000000+(stop.tv_usec-start.tv_usec);\
		printf("time use is %dus\n",time_use);\
	}\
	time_flag = 0;\
}
		

