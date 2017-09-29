#include"page_manager.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
KEYT DELETEDKEY;
KEYT getRPPA(segment *);
bool garbage_collect_data(segment *);
bool garbage_collect_tp(segment *);
KEYT getPPA(segment *input,bool);
void segment_init(segment *,int ,int);
void segment_block_init(segment*,int);
void segment_block_change(segment *, int);
void segment_free(segment *);
/*for user*/
void PM_init(PM* pm){
	pm->data_sets=(segment*)malloc(sizeof(segment));
	segment_init(pm->data_sets,0,SEGNUM-2);
	pm->tp_sets=(segment*)malloc(sizeof(segment));
	segment_init(pm->tp_sets,SEGNUM-1,1);
	pm->oob=(uint64_t*)malloc(sizeof(uint64_t)*MAXPAGE);
}
KEYT getDataPPA(PM *pm){
	return getPPA(pm->data_sets,true);
}
KEYT getTrPPA(PM *pm){
	return getPPA(pm->tp_sets,false);
}
void PM_free(PM *pm){
	segment_free(pm->data_sets);
	segment_free(pm->tp_sets);
	free(pm->oob);
	free(pm);
}
/*for user end*/


/*for intra*/
bool garbage_collect_data(segment *input){
	return false;
}
bool garbage_collect_tp(segment *input){
	return false;
}
void freePPA(segment *input, KEYT in){
	//queue_push(input->ppa,in);
	input->ppa->push(in);
}
KEYT getRPPA(segment* seg){
	KEYT res=seg->reserve_block*PAGENUM;
	res+=seg->reserve_block_point++;
	return res;
}

KEYT getPPA(segment *input, bool isdata){
	pthread_mutex_lock(&input->ppa_lock);
	bool flag=true;
	while(!input->ppa->size()){
		if(isdata){
			if(!(flag=garbage_collect_data(input)))
				break;
		}
		else{
			if(!(flag=garbage_collect_tp(input)))
				break;
		}
	}
	if(!flag){
		printf("full storage!\n");
		return 0;
	}
	KEYT res=input->ppa->front();
	input->ppa->pop();
	pthread_mutex_unlock(&input->ppa_lock);
	return res;
}
void segment_init(segment *input,int start_block_n,int size){
	DELETEDKEY=MAXPAGE+DTPBLOCK*PAGENUM;
	input->size=size;
	input->blocks=(block*)malloc(sizeof(block)*(size+1));
	input->ppa=new std::queue<KEYT>();
	
	for(int i=0;i<size+1; i++){
		for(int j=0; j<PAGENUM/8; j++){
			input->blocks[i].bitset[j]=0xFF;
		}
		input->blocks[i].invalid_n=0;
		input->blocks[i].number=start_block_n+i;
		input->blocks[i].erased=true;
	}
	input->reserve_block=start_block_n+size;

	/*ppa input*/
	int target_number=start_block_n*PAGENUM;
	for(int i=start_block_n; i<start_block_n+size; i++){
		for(int j=0; j<PAGENUM;j++){
			input->ppa->push(target_number++);
		}
	}

	pthread_mutex_init(&input->ppa_lock,NULL);
	input->start_block_n=start_block_n;
	input->reserve_block_point=0;
}
void segment_block_init(segment* input,int block_num){
	for(int j=0; j<PAGENUM/8; j++){
		input->blocks[block_num].bitset[j]=0xFF;
	}
	input->blocks[block_num].invalid_n=0;
	input->blocks[block_num].erased=true;
}
void segment_block_change(segment *input, int target_block){
	KEYT res=input->reserve_block * PAGENUM;
	for(int i=input->reserve_block_point; i<PAGENUM; i++){
		freePPA(input,res+i);
	}
	input->reserve_block=input->blocks[target_block].number;
	block temp;
	memcpy(&temp,&input->blocks[target_block],sizeof(block));
	memcpy(&input->blocks[target_block],&input->blocks[input->size],sizeof(block));
	memcpy(&input->blocks[input->size],&temp,sizeof(block));
	input->reserve_block_point=0;
}
void segment_free(segment *input){
	free(input->blocks);
	free(input);
}
/*for intra*/
