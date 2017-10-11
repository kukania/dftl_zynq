#include"LRU.h"
#include<stdlib.h>
#include<stdio.h>
void LRU_init(LRU *lru,int max){
	lru->top=NULL;
	lru->bottom=NULL;
	lru->m_size=max;
	lru->n_size=0;
}

entry *LRU_get(LRU *lru){
	if(lru->n_size==0) return NULL;
	lru_node *res=lru->bottom;
	lru_node *bef=res->up;
	entry *ent_res=res->target;
	if(bef==NULL){
		lru->bottom=lru->top;
		free(res);
	}
	else{
		bef->down=NULL;
		lru->bottom=bef;
		free(res);
	}
	lru->n_size--;
	return ent_res;
}

void LRU_insert(LRU *lru, entry *ent){
	if(lru->m_size==lru->n_size) return;
	lru_node *new_node=(lru_node*)malloc(sizeof(lru_node));
	new_node->target=ent;
	if(lru->bottom==NULL){
		lru->bottom=new_node;
		lru->top=new_node;
		lru->bottom->down=NULL;
		lru->top->up=NULL;
		lru->n_size++;
		return; 
	}
	LRU_delete(lru,ent);
	lru->top->up=new_node;
	new_node->down=lru->top;

	lru->top=new_node;
	lru->n_size++;
}

void LRU_delete(LRU *lru, entry *ent){
	
}

void LRU_free(LRU *lru){
	while(LRU_get(lru)){}
	free(lru);
}
#ifdef TEST_LRU
int main(){
	LRU *lru=(LRU*)malloc(sizeof(LRU));
	LRU_init(lru,100);
	int arr[]={0};
	entry *temp;
	for(int i=0; i<sizeof(arr)/sizeof(int); i++){
		temp=(entry *)malloc(sizeof(entry));
		temp->sets.ppa=arr[i];
		temp->sets.lpn=arr[i];
		LRU_insert(lru,temp);
	}
	while((temp=LRU_get(lru))){
		printf("%d\n",temp->sets.lpn);
		free(temp);
	}
	LRU_free(lru);
}
#endif
