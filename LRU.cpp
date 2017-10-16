#include"LRU.h"
#include<stdlib.h>
#include<stdio.h>
void LRU_init(LRU *lru,int max,skiplist *cbody){
	lru->top=NULL;
	lru->bottom=NULL;
	lru->m_size=max;
	lru->n_size=0;
	lru->c_body=cbody;
}

entry *LRU_get(LRU *lru){
	if(lru->n_size==0) return NULL;
	lru_node *res=lru->bottom;
	lru_node *up=res->up;
	entry *ent_res=res->target;
	if(up==NULL){
		lru->bottom=lru->top;
	}
	else{
		up->down=NULL;
		lru->bottom=up;
	}
	free(res);
	lru->n_size--;
	return ent_res;
}

lru_node* LRU_insert(LRU *lru, entry *ent){
	if(lru->m_size==lru->n_size) return NULL;
	lru_node *new_node=(lru_node*)malloc(sizeof(lru_node));
	new_node->target=ent;
	new_node->content=lru;
	if(lru->bottom==NULL){
		lru->bottom=new_node;
		lru->top=new_node;
		lru->bottom->down=NULL;
		lru->top->up=NULL;
		lru->n_size++;
		return new_node; 
	}
	lru->top->up=new_node;
	new_node->down=lru->top;

	lru->top=new_node;
	lru->n_size++;
	return new_node;
}

bool LRU_delete(lru_node *node){
	LRU *lru=node->content;
	if(lru->n_size==0) return false;

	if(lru->top==node){
		lru_node* down=node->down;
		down->up=NULL;
		lru->top=down;
	}
	else if(lru->bottom==node){
		lru_node *up=node->up;
		up->down=NULL;
		lru->bottom=up;
	}
	else if(lru->bottom==lru->top && lru->top==node){
		lru->bottom=NULL;
		lru->top=NULL;
	}
	else{
		lru_node *up=node->up;
		lru_node *down=node->down;

		up->down=down;
		down->up=up;
	}
	lru->n_size--;
	free(node);
	return true;
}

void LRU_update(lru_node *node){
	LRU *lru=node->content;
	if(lru->top==node) return;
	if(lru->bottom==node){
		lru_node *up=node->up;
		up->down=NULL;

		lru->bottom=up;
	}
	else{
		lru_node *up=node->up;
		lru_node *down=node->down;

		up->down=down;
		down->up=up;
	}
	lru->top->up=node;
	node->down=lru->top;
	lru->top=node;
}

void LRU_free(LRU *lru){
	/*
	lru_node *start=lru->top;
	if(!start){
		lru_node *next=start->down;
		while(LRU_delete(start)){
			start=next;
			next=start->down;
		}
	}*/
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
