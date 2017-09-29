#include"dftl.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

extern dftl_t dftl;
int main(){
	//setup
	dftl_init(&dftl);
	int *arr;
	if(!SEQUENCE){
	
	}
	//write
	for(int i=1; i<=INPUTSIZE;i++){
		char *data=(char *)malloc(PAGESIZE);
		memcpy(data,&i,sizeof(i));
		dftl_write(&dftl,i,data,NULL);
	}
	//read
	for(int i=1; i<=INPUTSIZE; i++){
		char *data=(char *)malloc(PAGESIZE);
		dftl_read(&dftl,i,data,NULL);
	}
	dftl_free(&dftl);
}
