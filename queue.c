#include"queue.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#define F 0
#define R 1
int CHECK(int a, int b){
	return ((a+1)%(b+1));
}
int CALC(queue *input,int value){
	if(value==F){
		input->f=(input->f+1)%(input->m_size+1);
		return input->f;
	}
	else{
		input->r=(input->r+1)%(input->m_size+1);
		return input->r;
	}
}


void queue_init(queue *input,int size){
	input->m_size=size;
	input->body=(KEYT *)malloc(sizeof(KEYT)*(size+1));
	input->f=0;
	input->r=0;
}

KEYT* queue_dequeue(queue *input){
	if(input->f==input->r){
		return NULL;
	}
	KEYT* res=&input->body[input->f];
	CALC(input,F);
	return res;
}

int queue_enqueue(queue *input, KEYT value){
	int test=CHECK(input->r,input->m_size);
	if(test==input->f)
		return 0;
	else{
		input->body[input->r]=value;
		CALC(input,R);
		return 1;
	}
}

void queue_free(queue *input){
	free(input->body);
	free(input);
}
#ifdef TEST_MAIN
int main(){
	queue* test=(queue*)malloc(sizeof(queue));
	queue_init(test,1000);

	int* res;
	for(int i=0; i<500; i++){
		if(!queue_enqueue(test,i)){
			printf("%d fail\n",i);
		}
	}
	while((res=queue_dequeue(test))){
		printf("%d\n",*res);
	}

	for(int i=500; i<2000; i++){
		if(!queue_enqueue(test,i)){
			printf("%d fail\n",i);
			break;
		}
	}
	while((res=queue_dequeue(test))){
		printf("%d\n",*res);
	}

	queue_free(test);
}


#endif
