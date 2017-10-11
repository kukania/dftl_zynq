#include"utils.h"
#include"cache.h"
#include"dftl.h"
#include<limits.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
extern dftl_t dftl;
void cache_init(cache *c){
	c->m_size=CACHEMEM/(sizeof(entry)+KEYN*sizeof(keyset));
	printf("cache page size : %d\n",c->m_size);
	c->n_size=0;
	c->cnt=1;
	c->body=skiplist_init();
	
	c->lru=(LRU*)malloc(sizeof(LRU));
	LRU_init(c->lru,c->m_size);
}

void cache_free(cache *c){
	skiplist_free(c->body,true);
	LRU_free(c->lru);
	free(c);
}

tp* cache_load(cache *c, KEYT target){
	int dirnum=convert_ptod(target);
	KEYT ppa=dftl.tp_dir.tp_location[dirnum];
	if(ppa==UINT_MAX)
		return NULL;
	return tp_read(ppa,dftl.fd);
}

entry* cache_insert(cache *c,keyset input,bool iswrite){
	if(c->m_size<=c->n_size){
		cache_evict(c);
	}
	int skip_key=((input.lpn-1)/KEYN)*KEYN+1; //start from 1
	entry *ent;
	snode *target;
	if(iswrite && (target=skiplist_find(c->body,skip_key))){
		ent=target->d.e_data;
		if(ent->e_dirty[(input.lpn-1)%KEYN]){
			//report ppa
		}
		ent->e_dirty[(input.lpn-1)%KEYN]=iswrite;
		ent->mapping->sets[(input.lpn-1)%KEYN]=input;
	}
	else{
		ent=(entry*)malloc(sizeof(entry));
		memset(ent,0,sizeof(ent));
		ent->mapping=cache_load(c,skip_key);
		if(ent->mapping==NULL){
			ent->mapping=(tp*)malloc(sizeof(tp));
			memset(ent->mapping,0,sizeof(tp));
		}
		if(iswrite)
			ent->mapping->sets[(input.lpn-1)%KEYN]=input;
		
		if(!ent->e_dirty[(input.lpn-1)%KEYN])
			ent->e_dirty[(input.lpn-1)%KEYN]=iswrite;
		snode *t=skiplist_insert(c->body,skip_key,ent,true);
		c->n_size++;
	}
	//LRU_insert(c->lru,input);
	ent->cnt=c->cnt++;
	if(!ent->dirty)
		ent->dirty=iswrite;
	return ent;
}

snode *cache_LRU(cache *c){
	if(c->n_size==0) return NULL;
	sk_iter *iter=skiplist_get_iterator(c->body);
	KEYT m_cnt=UINT_MAX;
	snode *res;
	snode *now;
	while((now=skiplist_get_next(iter))!=NULL){
		if(m_cnt>now->d.e_data->cnt){
			res=now;
			m_cnt=now->d.e_data->cnt;
		}
	}
	free(iter);
	return res;
}
int t;
void cache_evict(cache *c){
	//entry* target=LRU_get(c->lru);
	snode *res=cache_LRU(c);
	entry *target=res->d.e_data;
	if(target->dirty){
		int dirnum=convert_ptod(target->mapping->sets[0].lpn);
		tp_update(&dftl.tp_dir,dirnum,target->mapping,dftl.fd);
		free(res->d.e_data);
		res->d.e_data=NULL;
	}
	else{

	}
	int code;
	if((code=skiplist_delete(c->body,res->key))){
		printf("delete error : %d\n",code);
	}
	c->n_size--;
}

keyset* cache_find(cache *c,KEYT target){
	KEYT skip_key=((target-1)/KEYN)*KEYN+1;
	snode *temp_s=skiplist_find(c->body,skip_key);
	entry *res;
	if(temp_s==NULL){
		keyset temp;
		temp.lpn=target;
		temp.ppa=0;
		res=cache_insert(c,temp,false);
	}
	else{
		res=temp_s->d.e_data;
	}
	return &res->mapping->sets[(target-1)%KEYN];
}

