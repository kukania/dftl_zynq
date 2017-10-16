#include"dftl.h"
#include"utils.h"
#include"interface.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
extern dftl_t dftl;
MeasureTime wt,rt;
extern MeasureTime at;
extern processor process;
int main(){
	//setup
	measure_init(&wt);
	measure_init(&rt);
	processor_init();
	dftl_init(&dftl);
	srand(1);
	int *arr;
	if(!SEQUENCE){
		arr=(int*)malloc(sizeof(int)*INPUTSIZE);
		for(int i=0; i<INPUTSIZE; i++){
			arr[i]=rand()%INPUTSIZE+1;//start from 1 
		}
	}
	//write
	printf("write start!\n");
	MS(&wt);
	for(int i=1; i<=INPUTSIZE;i++){
		if(!(i%1024)){
			printf("%d done\n",i);
		}
		KEYT key;
		if(SEQUENCE){
			key=i;
		}
		else{
			key=arr[i];
		}
		char *data=(char *)malloc(PAGESIZE);
		memcpy(data,&key,sizeof(key));
		//dftl_write(&dftl,key,data,NULL);
		make_req(key,data,W_TYPE);
	}
	MA(&wt);
	
	//read
	printf("read start!\n");
	MS(&rt);
	for(int i=1; i<=INPUTSIZE; i++){
		if(!(i%1024)){
			printf("%d done\n",i);
		}
		KEYT key;
		if(SEQUENCE){
			key=i;
		}
		else{
			key=arr[i];
		}
		char *data=(char *)malloc(PAGESIZE);
		//dftl_read(&dftl,key,data,NULL);
		make_req(key,data,R_TYPE);
	}
	MA(&rt);
	processor_wait();
//	processor_free();
	printf("write time %ld & %f\n",wt.adding.tv_sec,(float)wt.adding.tv_usec/1000000);
	printf("read time %ld & %f\n",rt.adding.tv_sec,(float)rt.adding.tv_usec/1000000);
	printf("find time %ld & %f\n",at.adding.tv_sec,(float)at.adding.tv_usec/1000000);
	dftl_free(&dftl);
}
