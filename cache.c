#include"utils.h"
#include"cache.h"
#include"dftl.h"
#include<limits.h>
#include<stdlib.h>
#include<string.h>
extern dftl_t dftl;
void cache_init(cache *c, int fd){
	c->m_size=CACHEMEM/sizeof(entry);
	c->n_size=0;
	c->cnt=1;
	c->body=skiplist_init();

	c->lru=(LRU*)malloc(sizeof(LRU));
	c->fd=fd;
	LRU_init(c->lru,c->m_size);
}

void cache_free(cache *c){
	skiplist_free(c->body);
	LRU_free(c->lru);
	free(c);
}

void cache_insert(cache *c,entry *input){
	if(c->m_size<=c->n_size)
		cache_evict(c);
	skiplist_insert(c->body,input->sets.lpn,input);
	LRU_insert(c->lru,input);
}

void cache_evict(cache *c){
	entry* target=LRU_get(c->lru);
	tp* temp=(tp*)malloc(sizeof(tp));
	memset(temp,0,sizeof(tp));
	
	if(target->dirty){
		int dirnum=convert_ptod(target->sets.ppa);
		//start from 1
		for(int i=dirnum+1; i<dirnum+KEYN+1; i++){
			snode *temp_s=skiplist_find(c->body,i);
			if(temp_s){
				temp->sets[temp_s->data->sets.lpn%KEYN]=temp_s->data->sets;
				temp_s->data->dirty=false;
			}
		}
		tp_update(&dftl.tp_dir,dirnum,temp,c->fd);
		for(int i=0; i<KEYN; i++){
			if(!temp->sets[i].ppa && !temp->sets[i].lpn) continue;
			skiplist_delete(c->body,temp->sets[i].lpn);
		}
	}
	else{
		skiplist_delete(c->body,target->sets.lpn);
	}
	free(temp);
}

entry* cache_find(cache *c,KEYT target){
	snode *temp_s=skiplist_find(c->body,target);
	entry *ret=temp_s->data;
	if(ret){
		LRU_insert(c->lru,ret);
	}
	return ret;
}

