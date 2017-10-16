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
	//c->cnt=1;
	c->body=skiplist_init();
	
	c->lru=(LRU*)malloc(sizeof(LRU));
	LRU_init(c->lru,c->m_size,c->body);
}
void cache_free(cache *c){
	LRU_free(c->lru);
	skiplist_free(c->body,true);
	free(c);
}

bool cache_load(cache *c, KEYT target,inter_req *req){
	int dirnum=convert_ptod(target);
	KEYT ppa=dftl.tp_dir.tp_location[dirnum];
	if(ppa==UINT_MAX)
		return false;
	//if throw tp read req
	tp_read(ppa,dftl.fd,req);
	
	return true;
}

entry* cache_lookup(cache *c,inter_req *req,bool iswrite){
	int skip_key=((req->key-1)/KEYN)*KEYN+1; //start from 1
	entry *ent;
	snode *target;
	if((target=skiplist_find(c->body,skip_key))){
		ent=target->d.e_data;
		LRU_update(ent->l_entry);
		if(iswrite){
			if(ent->e_dirty[(req->key-1)%KEYN]){
				//report ppa for data
			}
			ent->e_dirty[(req->key-1)%KEYN]=iswrite;
			ent->mapping->sets[(req->key-1)%KEYN].lpn=req->key;
			ent->mapping->sets[(req->key-1)%KEYN].ppa=req->ppa;
		}
		else{
			
		}
		//lru_update
	}
	else{//if entry doesn't exist
		if(!cache_load(c,skip_key,req)){ // no mapping
			if(c->m_size<=c->n_size){
				cache_evict(c);
			}
			ent=(entry*)malloc(sizeof(entry));
			memset(ent->e_dirty,0,sizeof(ent->e_dirty));
			ent->dirty=false;
			ent->mapping=(tp*)malloc(sizeof(tp));
			memset(ent->mapping,0,sizeof(tp));
			ent->mapping->sets[(req->key-1)%KEYN].lpn=req->key;
			ent->mapping->sets[(req->key-1)%KEYN].ppa=req->ppa;
			ent->e_dirty[(req->key-1)%KEYN]=iswrite;
			snode *t=skiplist_insert(c->body,skip_key,ent,true);
			ent->key=skip_key;
			lru_node *lru_addr=LRU_insert(c->lru,ent);
			ent->l_entry=lru_addr;
			c->n_size++;
		}
		else {
			//if directory is exist, pipelining do rest work
			return NULL;
		}
	}
	//ent->cnt=c->cnt++;
	if(!ent->dirty)
		ent->dirty=iswrite;
	return ent;
}
void cache_processing(cache *c, inter_req *req){
	tp* mapping_data=(tp*)req->meta;
	keyset target_set;
	KEYT skip_key=mapping_data->sets[0].lpn;
	entry *ent=(entry*)malloc(sizeof(entry));
	memset(ent->e_dirty,0,sizeof(ent->e_dirty));
	switch(req->type){
		case W_TYPE:
			target_set.lpn=req->key;
			target_set.ppa=req->ppa;
			//update mapping data
			mapping_data->sets[(req->key-1)%KEYN]=target_set;
			ent->dirty=true;
			ent->e_dirty[(req->key-1)%KEYN]=true;
			dftl_writeTbuffer(&dftl,req);
			break;
		case R_TYPE:
			target_set=mapping_data->sets[(req->key-1)%KEYN];
			if(target_set.lpn!=req->key){
				printf("key error!\n");
			}
			inter_read(target_set.ppa,req->data,req);
			ent->dirty=false;
			break;
		default:
			printf("??");
			break;
	}
	if(c->m_size<=c->n_size){
		cache_evict(c);
	}
	ent->mapping=mapping_data;
	snode *t=skiplist_insert(c->body,skip_key,ent,true);
	ent->key=skip_key;
	lru_node *lru_addr=LRU_insert(c->lru,ent);
	ent->l_entry=lru_addr;
	c->n_size++;
}
/*
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
*/
void cache_evict(cache *c){
	entry* target=LRU_get(c->lru);//delete
	KEYT key=target->key;
	//snode *res=cache_LRU(c);
	//entry *target=res->d.e_data;
	if(target->dirty){
		int dirnum=convert_ptod(key);
		tp_update(&dftl.tp_dir,dirnum,target->mapping,dftl.fd);
		//in update free mapping information of entry
		target->mapping=NULL;
	}
	else{

	}
	int code;
	if((code=skiplist_delete(c->body,key))){
		printf("delete error : %d\n",code);
	}
	c->n_size--;
}
/*
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
		LRU_update(res->l_entry);
		//lru_update
	}
	return &res->mapping->sets[(target-1)%KEYN];
}*/
