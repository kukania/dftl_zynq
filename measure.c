
#include"measure.h"
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<string.h>
#include"utils.h"
void donothing(MeasureTime *t){
}
void donothing2(MeasureTime *t,char *a){
}
void measure_init(MeasureTime *m){
	m->header=NULL;
	m->cnt=0;
	m->adding.tv_sec=0;
	m->adding.tv_usec=0;
}
void measure_start(MeasureTime *m){
	linktime *new_val=(linktime*)malloc(sizeof(linktime));
	if(m->header==NULL){
		m->header=new_val;
	}
	else{
		new_val->next=m->header;
		m->header=new_val;
	}
	gettimeofday(&new_val->start,NULL);
	return;
}
void measure_pop(MeasureTime *m){
	linktime *t=m->header;
	m->header=m->header->next;
	free(t);
	return;
}
struct timeval measure_res(MeasureTime *m){
	struct timeval res; linktime *t;
	gettimeofday(&m->header->end,NULL);
	timersub(&m->header->end,&m->header->start,&res);
	t=m->header;
	m->header=m->header->next;
	free(t);
	return res;
}
void measure_adding(MeasureTime *m){
	struct timeval res; linktime *t;
	gettimeofday(&m->header->end,NULL);
	timersub(&m->header->end,&m->header->start,&res);
	m->adding.tv_sec+=res.tv_sec;
	m->adding.tv_usec+=res.tv_usec;
	t=m->header;
	m->header=m->header->next;
	free(t);
}
void measure_stamp(MeasureTime *m){
	struct timeval res; linktime *t;
	gettimeofday(&m->header->end,NULL);
	timersub(&m->header->end,&m->header->start,&res);
	printf("%ld,%.6f\n",res.tv_sec,(float)res.tv_usec/1000000);
	t=m->header;
	m->header=m->header->next;
	free(t);
}
/*
void measure_calc(MeasureTime *m){
	struct timeval res;
	gettimeofday(&m->header->end,NULL);
	timersub(&m->header->end,&m->header->start,&res);
	double num=res.tv_sec+((float)res.tv_usec/1000000);
	printf("%lf MB/s\n", ((double)(INPUTSIZE*PAGESIZE)/((1024*1024)*num)));
	linktime *t=m->header;
	m->header=m->header->next;
	free(t);
}*/
#ifdef NOHOST
void measure_end(MeasureTime *m,std::string input ){
	char format[200];
	memcpy(format,input.c_str(),input.length());
	format[input.length()]='\0';
	struct timeval res; linktime *t;
	gettimeofday(&m->header->end,NULL);
	timersub(&m->header->end,&m->header->start,&res);
	printf("%s:%ld sec and %.6f\n",format,res.tv_sec,(float)res.tv_usec/1000000);
	t=m->header;
	m->header=m->header->next;
	free(t);
	return;
}
#else
void measure_end(MeasureTime *m,char *format){
	struct timeval res; linktime *t;
	gettimeofday(&m->header->end,NULL);
	timersub(&m->header->end,&m->header->start,&res);
	printf("%s:%ld sec and %.6f\n",format,res.tv_sec,(float)res.tv_usec/1000000);
	t=m->header;
	m->header=m->header->next;
	free(t);
	return;
}
#endif

#ifdef DEBUG
int main(){
	MeasureTime t;
	measure_init(&t);
	measure_start(&t);
	measure_end(&t);
}
#endif
