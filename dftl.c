#include"dftl.h"
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
dftl_t dftl;
void dftl_init(dftl_t *input){
	memset(input,0,sizeof(dftl_t));
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
	cache_init(input->map,fd);
	input->pm=(PM*)malloc(sizeof(PM));
	PM_init(input->pm);
}
void dftl_free(dftl_t *input){
	cache_free(input->map);
	PM_free(input->pm);
	//free(input);
}
void dftl_write(dftl_t *d,KEYT key, char *data, inter_req *req){
	KEYT data_ppa=getDataPPA(d->pm);//get ppa from manager
	inter_write(data_ppa,data,req);//data write
	entry* temp_ent=cache_find(d->map,key);
	if(temp_ent==NULL){
		temp_ent=(entry *)malloc(sizeof(entry));
		temp_ent->sets.ppa=data_ppa;
		temp_ent->sets.lpn=key;
		cache_insert(d->map,temp_ent);
	}	
	else{
		/*report old ppn is deprecated*/
		temp_ent->sets.ppa=data_ppa;
	}
	temp_ent->dirty=true;
}
void dftl_read(dftl_t *d,KEYT key, char *data, inter_req *req){
	entry *temp_ent=cache_find(d->map,key);
	if(!temp_ent){
		keyset *tp_rot=tp_find_map(&d->tp_dir,key);
		tp* temp_tp=tp_read(tp_rot->ppa,d->map->fd);
		keyset* target=tp_find_key(temp_tp,key);
		temp_ent=(entry *)malloc(sizeof(entry));
		temp_ent->sets=*target;
		temp_ent->dirty=false;
		cache_insert(d->map,temp_ent);
		free(temp_tp);
	}
	inter_read(temp_ent->sets.ppa,data,req);
}
void dftl_trim(dftl_t *d,KEYT key ,char *data, inter_req *req){

}
