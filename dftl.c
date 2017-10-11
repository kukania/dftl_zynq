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
void dftl_write(dftl_t *d,KEYT key, char *data, inter_req *req){
	if(d->w_buffer->size>=KEYN){//if write buffer full
		sk_iter *iter=skiplist_get_iterator(d->w_buffer);
		snode *now;
		w_entry *w_temp;
		while((now=skiplist_get_next(iter))!=NULL){
			w_temp=now->d.w_data;
			keyset value;
			value.lpn=now->key;
			value.ppa=getDataPPA(d->pm);//get ppa from manage
			inter_write(value.ppa,w_temp->data,w_temp->req);
			w_temp->data=NULL;
			cache_insert(d->map,value,true);
		}
		free(iter);
		skiplist_free(d->w_buffer,false);
		d->w_buffer=skiplist_init();
	}
	w_entry* new_entry=(w_entry*)malloc(sizeof(w_entry));
	new_entry->data=data;
	new_entry->req=req;
	skiplist_insert(d->w_buffer,key,(void*)new_entry,false);
}
void dftl_read(dftl_t *d,KEYT key, char *data, inter_req *req){
	snode *target=skiplist_find(d->w_buffer,key);
	if(target){
#ifdef NOINTER
		free(data);
#endif
		return;
	}

	keyset *temp_keyset=cache_find(d->map,key);
	if(temp_keyset->lpn!=key){
		printf("key error!\n");
	}
	inter_read(temp_keyset->ppa,data,req);
#ifdef NOINTER
	free(data);
#endif
}
void dftl_trim(dftl_t *d,KEYT key ,char *data, inter_req *req){

}
