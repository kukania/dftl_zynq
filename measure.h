#ifndef __MEASURE_H__
#define __MEASURE_H__
#include<sys/time.h>
typedef struct linktime{
	struct timeval start,end;
	struct linktime * next;
}linktime;

typedef struct MeasureTime{
	linktime *header;
	struct timeval adding;
	int call;
	int cnt;
}MeasureTime;

void measure_init(MeasureTime *);
void measure_start(MeasureTime *);
void measure_pop(MeasureTime *);
void measure_stamp(MeasureTime *);
void measure_adding(MeasureTime *);
struct timeval measure_res(MeasureTime *);
#ifdef NOHOST
#include<string>
void measure_end(MeasureTime *,std::string);
#else
void measure_end(MeasureTime *,char *);
#endif
void measure_calc(MeasureTime *);
void donothing(MeasureTime *t);
void donothing2(MeasureTime *t,char *a);
#endif
