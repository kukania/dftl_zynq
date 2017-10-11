#include"dftl.h"
#include"utils.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

extern dftl_t dftl;
MeasureTime wt,rt;
extern MeasureTime at;
int main(){
	//setup
	measure_init(&wt);
	measure_init(&rt);
	dftl_init(&dftl);
	int *arr;
	if(!SEQUENCE){
	
	}
	//write
	printf("write start!\n");
	MS(&wt);
	for(int i=1; i<=INPUTSIZE;i++){
		if(!(i%1024)){
		//	printf("%d done\n",i);
		}
		char *data=(char *)malloc(PAGESIZE);
		memcpy(data,&i,sizeof(i));
		dftl_write(&dftl,i,data,NULL);
	}
	MA(&wt);
	
	//read
	printf("read start!\n");
	MS(&rt);
	for(int i=1; i<=INPUTSIZE; i++){
		if(!(i%1024)){
	//		printf("%d done\n",i);
		}
		char *data=(char *)malloc(PAGESIZE);
		dftl_read(&dftl,i,data,NULL);
	}
	MA(&rt);

	printf("write time %ld & %f\n",wt.adding.tv_sec,(float)wt.adding.tv_usec/1000000);
	printf("read time %ld & %f\n",rt.adding.tv_sec,(float)rt.adding.tv_usec/1000000);
	printf("find time %ld & %f\n",at.adding.tv_sec,(float)at.adding.tv_usec/1000000);
	dftl_free(&dftl);
}
