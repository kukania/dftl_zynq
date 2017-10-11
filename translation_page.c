#include"dftl.h"
#include"page_manager.h"
#include"translation_page.h"
#include"interface.h"
#include<stdlib.h>
#include<stdio.h>
extern dftl_t dftl;
keyset* tp_find_key(translation_page *input, KEYT key){
	return &input->sets[(key-1)%KEYN];
}
int convert_ptod(KEYT ppa){
	return (ppa-1)/KEYN;
}
KEYT tp_find_map(directory *dir, KEYT key){
	int number=convert_ptod(key);
	return dir->tp_location[number];
}

KEYT tp_write(tp* input, int fd){
	inter_req *tp_req=(inter_req*)malloc(sizeof(inter_req));
	tp_req->type=TP_W_TYPE;
	tp_req->ppa=getTrPPA(dftl.pm);//get ppa from manager
	tp_req->data=(char*)input;
	tp_req->fd=fd;
	tp_req->end_req=end_req;
	inter_write(tp_req->ppa,tp_req->data,tp_req);
	return tp_req->ppa;
}

tp* tp_read(KEYT ppa,int fd){
	tp * res=(tp*)malloc(sizeof(translation_page));
	inter_req *tp_req=(inter_req*)malloc(sizeof(inter_req));
	tp_req->type=TP_R_TYPE;
	tp_req->ppa=ppa;
	tp_req->end_req=end_req;
	pthread_mutex_init(&tp_req->lock,NULL);
#ifdef ENABLE_LIBFTL
	#ifdef ASYNC
		
	#else
		pthread_mutex_lock(&tp_req->lock);
	#endif
#else
	tp_req->data=(char*)res;
#endif
	inter_read(tp_req->ppa,tp_req->data,tp_req);
	pthread_mutex_lock(&tp_req->lock);
	return (tp*)res;
}

void tp_update(directory *dir, int dirnum, tp* target,int fd){
	tp *read_s;
	//printf("cache_update: %d\n",target->sets[0].lpn);
	if(dir->tp_location[dirnum]!=UINT_MAX){//new table write
		//report invalid ppa in header
	}
	read_s=target;
	/*
	else{//old table is existed
		read_s=tp_read(dir->tp_location[dirnum],fd);
	}
	int target_idx=0;
	for(int i=0; i<KEYN; i++){
		if(target->sets[i].lpn==0 && target->sets[i].ppa==0)
			continue;
		else{
			read_s->sets[i]=target->sets[i];
		}
	}*/
	KEYT after_ppa=tp_write(read_s,fd);
	dir->tp_location[dirnum]=after_ppa;
}
