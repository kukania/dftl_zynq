#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<unistd.h>
#include<sys/types.h>
#include"utils.h"
#include"skiplist.h"
skiplist *skiplist_init(){
	skiplist *point=(skiplist*)malloc(sizeof(skiplist));
	point->level=1;
	point->header=(snode*)malloc(sizeof(snode));
	point->header->list=(snode**)malloc(sizeof(snode)*(MAX_L+1));
	for(int i=0; i<MAX_L; i++) point->header->list[i]=point->header;
	point->header->key=INT_MAX;
	point->size=0;
	return point;
}

snode *skiplist_find(skiplist *list, KEYT key){
	if(list->size==0) return NULL;
	snode *x=list->header;
	for(int i=list->level; i>=1; i--){
		while(x->list[i]->key<key)
			x=x->list[i];
	}
	if(x->list[1]->key==key)
		return x->list[1];
	return NULL;
}

static int getLevel(){
	int level=1;
	int temp=rand();
	while(temp % PROB==1){
		temp=rand();
		level++;
		if(level+1>=MAX_L) break;
	}
	return level;
}
snode *skiplist_insert(skiplist *list,KEYT key, void *value,bool isdata){
	snode *update[MAX_L+1];
	snode *x=list->header;
	for(int i=list->level; i>=1; i--){
		while(x->list[i]->key<key)
			x=x->list[i];
		update[i]=x;
	}
	x=x->list[1];

	if(key==x->key){
		if(value!=NULL){
			if(isdata){
				printf("double entries in body!\n");
				x->d.e_data=(entry*)value;
			}
			else
				x->d.w_data=(w_entry*)value;
		}
		return x;
	}
	else{
		int level=getLevel();
		if(level>list->level){
			for(int i=list->level+1; i<=level; i++){
				update[i]=list->header;
			}
			list->level=level;
		}

		x=(snode*)malloc(sizeof(snode));
		x->list=(snode**)malloc(sizeof(snode*)*(level+1));
		x->key=key;
		if(isdata)
			x->d.e_data=(entry*)value;
		else
			x->d.w_data=(w_entry*)value;

		for(int i=1; i<=level; i++){
			x->list[i]=update[i]->list[i];
			update[i]->list[i]=x;
		}
		x->level=level;
		list->size++;
	}
	return x;
}
int skiplist_delete(skiplist* list, KEYT key){
	if(list->size==0)
		return -1;
	snode *update[MAX_L+1];
	snode *x=list->header;
	for(int i=list->level; i>=1; i--){
		while(x->list[i]->key<key)
			x=x->list[i];
		update[i]=x;
	}
	x=x->list[1];

	if(x->key!=key)
		return -2; 

	for(int i=x->level; i>=1; i--){
		update[i]->list[i]=x->list[i];
		if(update[i]==update[i]->list[i])
			list->level--;
	}
	if(x->d.e_data){
		if(x->d.e_data->mapping!=NULL)
			free(x->d.e_data->mapping);
		free(x->d.e_data);
	}
	free(x->list);
	free(x);
	list->size--;
	return 0;
}

sk_iter* skiplist_get_iterator(skiplist *list){
	sk_iter *res=(sk_iter*)malloc(sizeof(sk_iter));
	res->list=list;
	res->now=list->header;
	return res;
}

snode *skiplist_get_next(sk_iter* iter){
	if(iter->now->list[1]==iter->list->header){ //end
		return NULL;
	}
	else{
		iter->now=iter->now->list[1];
		return iter->now;
	}
}
// for test
void skiplist_dump(skiplist * list){
	sk_iter *iter=skiplist_get_iterator(list);
	snode *now;
	while((now=skiplist_get_next(iter))!=NULL){
		for(int i=1; i<=now->level; i++){
			printf("%u ",now->key);
		}
		printf("\n");
	}
	free(iter);
}

void skiplist_clear(skiplist *list,bool isdata){
	snode *now=list->header->list[1];
	snode *next=now->list[1];
	while(now!=list->header){
		if(isdata){
			free(now->d.e_data->mapping);
			free(now->d.e_data);
		}
		else{
			//more!
			if(now->d.w_data->req){
				free(now->d.w_data->req->data);
				free(now->d.w_data->req);
			}
			free(now->d.w_data);
		}
		free(now->list);
		free(now);
		now=next;
		next=now->list[1];
	}
	list->size=0;
	list->level=0;
	for(int i=0; i<MAX_L; i++) list->header->list[i]=list->header;
	list->header->key=INT_MAX;
	
}
void skiplist_free(skiplist *list,bool isdata){
	skiplist_clear(list,isdata);
	free(list->header->list);
	free(list->header);
	free(list);
	return;
}

