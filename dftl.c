#include"dftl.h"
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
dftl_t dftl;
MeasureTime at;
void dftl_init(dftl_t *input){
	memset(input,0,sizeof(dftl_t));
	measure_init(&at);
	int fd;
	if(SEQUENCE){
		fd=open("data/dftl_s.data",O_RDWR|O_CREAT|O_TRUNC,0666);
	}
	else{
		fd=open("data/dftl_r.data",O_RDWR|O_CREAT|O_TRUNC,0666);
	}
	if(fd==-1){
		printf("file open error!\n");
		exit(-1);
	}
	input->map=(cache*)malloc(sizeof(cache));
	cache_init(input->map);
	input->pm=(PM*)malloc(sizeof(PM));
	PM_init(input->pm);
	for(int i=0; i<DIRNUM; i++){
		input->tp_dir.tp_location[i]=UINT_MAX;
	}
	input->fd=fd;
	input->w_buffer=skiplist_init();
}
void dftl_free(dftl_t *input){
	cache_free(input->map);
	PM_free(input->pm);
	skiplist_free(input->w_buffer,false);
	//free(input);
}
void dftl_writeTbuffer(dftl_t *d, inter_req* req){
	if(d->w_buffer->size>=KEYN){
		sk_iter *iter=skiplist_get_iterator(d->w_buffer);
		snode *now;
		w_entry *w_temp;
		inter_req *r_temp;
		while((now=skiplist_get_next(iter))!=NULL){
			w_temp=now->d.w_data;
			r_temp=w_temp->req;
			inter_write(r_temp->ppa,w_temp->data,r_temp);
			now->d.w_data->req=NULL;
		}
		free(iter);
		skiplist_free(d->w_buffer,false);
		d->w_buffer=skiplist_init();
	}
	w_entry *ent=(w_entry*)malloc(sizeof(w_entry));
	ent->data=req->data;
	ent->req=req;
	skiplist_insert(d->w_buffer,req->key,(void*)ent,false);
}
void dftl_write(dftl_t *d,KEYT key, char *data, inter_req *req){
	req->key=key;
	req->ppa=getDataPPA(d->pm);
	if(!cache_lookup(d->map,req,true)){ //deffered write
		return;
	}
	else{
		//write_buffer;
		dftl_writeTbuffer(d,req);
		return;
	}
}
void dftl_read(dftl_t *d,KEYT key, char *data, inter_req *req){
	snode *target=skiplist_find(d->w_buffer,key);
	//find write_buffer;
	if(target){
#ifdef NOINTER
		free(data);
#endif
		return;
	}
	entry *t=cache_lookup(d->map,req,false);
	if(t!=NULL){
		keyset temp_keyset=t->mapping->sets[(req->key-1)%KEYN];
		if(temp_keyset.lpn!=key){
			printf("key error!\n");
		}
		inter_read(temp_keyset.ppa,data,req);
	}
#ifdef NOINTER
	free(data);
#endif
}

void dftl_read_data(dftl_t *d, KEYT key ,char *data, inter_req *req){
}
void dftl_trim(dftl_t *d,KEYT key ,char *data, inter_req *req){

}
