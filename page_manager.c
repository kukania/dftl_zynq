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

int test_ppa;
void PM_init(PM* pm){
	DELETEDKEY=MAXPAGE+DTPBLOCK*PAGENUM;
	pm->data_sets=(segment*)malloc(sizeof(segment));
	segment_init(pm->data_sets,0,SEGNUM-2);
	pm->tp_sets=(segment*)malloc(sizeof(segment));
	segment_init(pm->tp_sets,SEGNUM-1,1);
	pm->oob=(uint64_t*)malloc(sizeof(uint64_t)*MAXPAGE);
	test_ppa=0;
}
KEYT getDataPPA(PM *pm){
	return test_ppa++;
	KEYT test=getPPA(pm->data_sets,true);
	return test;
}
KEYT getTrPPA(PM *pm){
	return test_ppa++;
	KEYT test=getPPA(pm->tp_sets,false);
	return test;
}
void PM_free(PM *pm){
	segment_free(pm->data_sets);
	segment_free(pm->tp_sets);
	free(pm->oob);
	free(pm);
}
void deletePPA(PM* pm, KEYT input){
	int block_num=input/PAGENUM;
	int offset_meta=input%PAGENUM;

	int bit_num=offset_meta/8;
	int offset=offset_meta%8;

	segment *set;
	if(pm->data_sets->start_block_n+pm->data_sets->size>block_num){
		set=pm->data_sets;
	}
	else{
		set=pm->tp_sets;
	}
	
	uint8_t target=1;
	uint8_t test=1;
	target=target<<offset;
	test=target;
	target=~target;

	for(int i=0; i<set->size; i++){
		if(set->blocks[i].number!=block_num) continue;
		if(!(test&set->blocks[i].bitset[bit_num])) continue;
		set->blocks[i].bitset[bit_num] &=target;
		set->blocks[i].invalid_n++;
		if(set->blocks[block_num].invalid_n>PAGENUM){
			printf("over block!]: %d\n",set->blocks[block_num].number);
			exit(1);
		}
	}
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
	queue_enqueue(input->ppa,in);
}
KEYT getRPPA(segment* seg){
	KEYT res=seg->reserve_block*PAGENUM;
	res+=seg->reserve_block_point++;
	return res;
}

KEYT getPPA(segment *input, bool isdata){
	pthread_mutex_lock(&input->ppa_lock);
	bool flag=true;
	KEYT *res;
	while(!(res=queue_dequeue(input->ppa))){
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
		printf("%s:full storage!\n",(isdata?"from data":"from header"));
		return 0;
	}
	pthread_mutex_unlock(&input->ppa_lock);
	return *res;
}
void segment_init(segment *input,int start_block_n,int size){
	input->size=size;
	input->blocks=(block*)malloc(sizeof(block)*(size+1));
	input->ppa=(queue*)malloc(sizeof(queue));
	queue_init(input->ppa,size*PAGENUM);
	
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
			queue_enqueue(input->ppa,target_number++);
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
	queue_free(input->ppa);
	pthread_mutex_destroy(&input->ppa_lock);
	free(input);
}
/*for intra*/
